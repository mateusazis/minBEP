#include "../include/SPScene.h"
#include "../include/Input.h"
#include <algorithm>
using namespace std;

MyScene::MyScene() : EarClippingTriangulationScene(){
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
}

void MyScene::render(float delta){
	glLineWidth(3);
	EarClippingTriangulationScene::render(delta);

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

	if (testPoints.size() >= 2){
		if (Input::checkKey('t')){
			testPoints[1] = Input::mousePosition();
			funnels.clear();
			sp = SP(testPoints[0], testPoints[1], points.data(), points.size(), funnels);
			currFunnel = 0;
		}

		glBegin(GL_POINTS);
		glColor3f(0, 1, 0);
		glVertex2fv(testPoints[0].data());
		glVertex2fv(testPoints[1].data());
		glEnd();

		//Draw graph
		vector<int> path = DepthFirstSearch(testPoints[0], testPoints[1], points.data(), dualGraph, triangles);
		glBegin(GL_LINE_STRIP);
		for (int t : path){
			glVertex2fv(getCenter(t).data());
		}
		glEnd();


	}

	//draw funnels
	if (funnels.size() > 0){
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

	}

	//DRAW TRIANGLE NUMBERS
	char temp1[3];
	glColor3f(0, 1, 0);
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

	if (testPoints.size() >= 2){
		//Draw path
		glBegin(GL_LINE_STRIP);
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex2fv(testPoints[0].data());
		for (int i : sp)
			glVertex2fv(points[i].data());
		glVertex2fv(testPoints[1].data());
		glEnd();
	}
}

vec2 MyScene::getCenter(int triangleIndex){
	triangleIndex *= 3;
	return (points[triangles[triangleIndex]] + points[triangles[triangleIndex + 1]] + points[triangles[triangleIndex + 2]]) / 3.0f;
}

void MyScene::onPointAdded(){
	EarClippingTriangulationScene::onPointAdded();
	dualGraph = getDualGraph(triangles);
	if (testPoints.size() >= 2){
		sp = SP(testPoints[0], testPoints[1], points.data(), points.size(), funnels);
	}

}

void MyScene::onKey(char c){
		EarClippingTriangulationScene::onKey(c);
	if (c == 'r'){
		points.clear();
		testPoints.clear();
		dualGraph.clear();
		triangles.clear();
		funnels.clear();
		currFunnel = 0;
	}
	if (Input::checkKeyDown('+'))
		currFunnel = std::min<int>(currFunnel + 1, funnels.size() - 1);
	if (Input::checkKeyDown('-'))
		currFunnel = max<int>(currFunnel - 1, 0);

	if (Input::checkKeyDown('s'))
		saveToFile();
	if (Input::checkKeyDown('l'))
		loadFromFile();
}

void MyScene::saveToFile(){
	FILE* f = fopen("points", "wb");
	int size = points.size();
	fwrite(&size, sizeof(int), 1, f);
	fwrite(points.data(), sizeof(vec2), points.size(), f);
	fclose(f);
}

void MyScene::loadFromFile(){
	FILE* f = fopen("points", "rb");
	int count;
	fread(&count, sizeof(int), 1, f);
	points.resize(count);
	fread(points.data(), sizeof(vec2), count, f);
	fclose(f);
	onPointAdded();
}

void MyScene::onMouseDown(){

	if (Input::checkKey('d')){
		testPoints.push_back(Input::mousePosition());
		if (testPoints.size() >= 2){
			sp = SP(testPoints[0], testPoints[1], points.data(), points.size(), funnels);
		}
	}
	else {
		EarClippingTriangulationScene::onMouseDown();
	}
}