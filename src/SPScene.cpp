#include "../include/SPScene.h"
#include "../include/Input.h"
#include "../include/Geometry.h"

#include <GL/glui.h>
#include <GL/freeglut.h>

#include <algorithm>

using namespace std;

extern int mainWindowID;

static SPScene* _s;
static void _resetPath(int){
	_s->resetPath();
}

static void _clear(int){
	_s->clear();
}

enum Mode { CREATE_POLYGON, EDIT_PATH };

SPScene::SPScene() : Scene(),
showTriangulation(1), showGraph(0), showTree(0), showFunnels(0), showNumbers(1), mode(CREATE_POLYGON){
	points.push_back(vec2(300, 300)); //0
	points.push_back(vec2(200, 300)); //1
	points.push_back(vec2(250, 250)); //2
	points.push_back(vec2(100, 200)); //3
	points.push_back(vec2(220, 150)); //4
	points.push_back(vec2(200, 100)); //5
	points.push_back(vec2(300, 100)); //6
	
	int tris[] = { 0, 1, 2, 0, 2, 4, 2, 3, 4, 0, 4, 6, 4, 5, 6 };
	for (int i : tris)
		triangles.push_back(i);
	dualGraph = getDualGraph(triangles);

	GLUI* g = GLUI_Master.create_glui_subwindow(mainWindowID, GLUI_SUBWINDOW_RIGHT);
	GLUI_Panel* visibilityPanel = g->add_panel("Show");
	g->add_checkbox_to_panel(visibilityPanel, "Triangulation", &showTriangulation);
	g->add_checkbox_to_panel(visibilityPanel, "Graph", &showGraph);
	g->add_checkbox_to_panel(visibilityPanel, "Tree", &showTree);
	g->add_checkbox_to_panel(visibilityPanel, "Funnels", &showFunnels);
	g->add_checkbox_to_panel(visibilityPanel, "Numbers", &showNumbers);
	g->add_button("Clear polygon", -1, &_clear);
	g->add_button("Clear path", -1, &_resetPath);
	g->set_main_gfx_window(mainWindowID);

	GLUI_Panel *panel = g->add_panel("Mode");
	GLUI_RadioGroup* modesGroup = g->add_radiogroup_to_panel(panel, &this->mode);
	g->add_radiobutton_to_group(modesGroup, "Create Polygon");
	g->add_radiobutton_to_group(modesGroup, "Set Path");

	_s = this;
}

void SPScene::resetPath(){
	testPoints.clear();
	funnels.clear();
}

//BEGIN DRAWING =============================================================================

void SPScene::render(){
	if (testPoints.size() >= 2){
		if (mode == EDIT_PATH){
			testPoints[1] = Input::mousePosition();
			funnels.clear();
			sp = SP(testPoints[0], testPoints[1], points.data(), points.size(), funnels);
			currFunnel = max<int>(0, (min<int>(funnels.size() - 1, currFunnel)));
		}
	}

	
	drawPolygon();
	glLineWidth(3);
	if (showGraph)
		drawGraph();
	if (showTree)
		drawDFSTree();
	if (showFunnels)
		drawFunnels();
	if (showNumbers)
		drawVertexNumbers();
	drawSP();
}

void SPScene::drawSP(){
	if (testPoints.size() >= 2){
		glBegin(GL_LINE_STRIP);
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex2fv(testPoints[0].data());
		for (int i : sp)
			glVertex2fv(points[i].data());
		glVertex2fv(testPoints[1].data());
		glEnd();
	}
}

void SPScene::drawVertexNumbers(){
	char temp1[3];
	glColor3f(0, .75f, 0);
	for (int i = 0; i < points.size(); i++){
		glRasterPos2fv(points[i].data());
		itoa(i, temp1, 10);
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)temp1);
	}

	if (testPoints.size() > 1){
		glRasterPos2fv(testPoints[0].data());
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"-1");
		glRasterPos2fv(testPoints[1].data());
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Dest");
	}
}

void SPScene::drawPolygon(){
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (points.size() > 2){
		glBegin(GL_TRIANGLES);
		glColor3f(1, 1, 1);
		for (int i = 0; i < triangles.size(); i++)
			glVertex2fv(points[triangles[i]].data());
		glEnd();
	}
	else {
		glBegin(points.size() == 2 ? GL_LINE_STRIP : GL_POINTS);
		glColor3f(1, 0, 0);
		for (vec2 p : points)
			glVertex2fv(p.data());
		glEnd();
	}

	if (showTriangulation){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(2);
		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);
		for (int i = 0; i < triangles.size(); i++)
			glVertex2fv(points[triangles[i]].data());
		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
}

void SPScene::drawGraph(){
	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	for (int i = 0; i < dualGraph.size(); i++){
		for (int j : dualGraph[i]){
			vec2 a = getCenter(i);
			vec2 b = getCenter(j);
			glVertex2fv(a.data());
			glVertex2fv(b.data());
		}
	}
	glEnd();
}

void SPScene::drawDFSTree(){
	if (testPoints.size() > 1){
		vector<int> path = DepthFirstSearch(testPoints[0], testPoints[1], points.data(), dualGraph, triangles);
		glColor3f(0, 1, 0);
		glBegin(GL_LINE_STRIP);
		for (int t : path){
			glVertex2fv(getCenter(t).data());
		}
		glEnd();
	}
}

void SPScene::drawFunnels(){
	if (funnels.size() > 0){
		glLineWidth(6);
		glBegin(GL_LINE_STRIP);
		glColor3f(.3f, .2f, 1);
		for (int i : funnels[currFunnel]){
			vec2 v;
			if (i == -1)
				v = testPoints[0];
			else
				v = points[i];
			glVertex2fv(v.data());
		}
		glEnd();
		glLineWidth(3);
	}
}

//END DRAWING =============================================================================

vec2 SPScene::getCenter(int triangleIndex){
	triangleIndex *= 3;
	return (points[triangles[triangleIndex]] + points[triangles[triangleIndex + 1]] + points[triangles[triangleIndex + 2]]) / 3.0f;
}

void SPScene::onPointAdded(){
	triangles = earClippingTriangulate(points.data(), points.size());
	dualGraph = getDualGraph(triangles);
	if (testPoints.size() >= 2){
		sp = SP(testPoints[0], testPoints[1], points.data(), points.size(), funnels);
	}

}

void SPScene::onKey(char c){
	if (Input::checkKeyDown('+'))
		currFunnel = std::min<int>(currFunnel + 1, funnels.size() - 1);
	if (Input::checkKeyDown('-'))
		currFunnel = max<int>(currFunnel - 1, 0);

	if (Input::checkKeyDown('s'))
		saveToFile();
	if (Input::checkKeyDown('l'))
		loadFromFile();

	if (c == 'c')
		system("cls");
}

void SPScene::clear(void){
	points.clear();
	testPoints.clear();
	dualGraph.clear();
	triangles.clear();
	funnels.clear();
	currFunnel = 0;
	glutPostRedisplay();
}

void SPScene::saveToFile(){
	FILE* f = fopen("points", "wb");
	int size = points.size();
	fwrite(&size, sizeof(int), 1, f);
	fwrite(points.data(), sizeof(vec2), points.size(), f);
	fclose(f);
}

void SPScene::loadFromFile(){
	FILE* f = fopen("points", "rb");
	int count;
	fread(&count, sizeof(int), 1, f);
	points.resize(count);
	fread(points.data(), sizeof(vec2), count, f);
	fclose(f);
	onPointAdded();
}

void SPScene::onMouseDown(){
	if (mode == EDIT_PATH){
		if (testPoints.size() < 2)
			testPoints.resize(2);
		testPoints[0] = testPoints[1] = Input::mousePosition();

		if (testPoints.size() >= 2){
			sp = SP(testPoints[0], testPoints[1], points.data(), points.size(), funnels);
		}
	}
	else if(mode == CREATE_POLYGON){
		points.push_back(Input::mousePosition());
		onPointAdded();
	}
}