#include "../include/Scenes.h"
#include "../include/lista1.h"
#include "../include/Input.h"
#include "../include/GeneralProblems.h"

using namespace std;

const static GLfloat successColors[2][3] = {
	1, 0, 0,
	0, 1, 0
};

ConvexityScene::ConvexityScene(){
	glOrtho(-SIZE, SIZE, -SIZE, SIZE, -1, 1);
	u = vec2(2, 1);
	v = vec2(1, 2);
}

void ConvexityScene::render(float delta){
	glBegin(GL_LINES);

	glColor3f(1, 1, 1);
	glVertex2f(0, 0);
	glVertex2fv(u.data());
	glVertex2f(0, 0);
	glVertex2fv(v.data());

	vec2 mouse = Input::mousePosition();
	vec2 w = vec2((mouse.x() - 200) / 100.0f * SIZE, (mouse.y() - 200) / 100.0f * SIZE);
	Convexidade c = getConvexity(u, v, w);
	glColor3fv(successColors[c == SIM]);
	glVertex2f(0, 0);
	glVertex2fv(w.data());
	glEnd();
}

void ConvexityScene::onKey(char c){
	const float OFFSET = 3 * 3.14f / 180.0f;
	if (c == '+')
		v = mat3::rotation(OFFSET) * v;
	if (c == '-')
		v = mat3::rotation(-OFFSET) * v;
}

DisjointsTriangles::DisjointsTriangles(){
	const float DEGREES_120 = 3.14f * 2.0f / 3.0f;
	triA[0] = triB[0] = vec2(0, 1);
	triA[1] = triB[1] = mat3::rotation(DEGREES_120) * triA[0];
	triA[2] = triB[2] = mat3::rotation(2 * DEGREES_120) * triA[0];

	mat3 bTranslation = mat3::translation(0.5f, 0.5f);
	triB[0] = bTranslation * triB[0];
	triB[1] = bTranslation * triB[1];
	triB[2] = bTranslation * triB[2];
	glEnable(GL_VERTEX_ARRAY);
}

void DisjointsTriangles::render(float delta){
	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 1);
	glVertex2fv(triA[0].data());
	glVertex2fv(triA[1].data());
	glVertex2fv(triA[2].data());
	bool disjoints = disjointsTriangles(triA, triB);
	/*glColor3f(!disjoints, disjoints, 0);*/
	glColor3fv(successColors[disjoints]);
	glVertex2fv(triB[0].data());
	glVertex2fv(triB[1].data());
	glVertex2fv(triB[2].data());
	glEnd();
}

void DisjointsTriangles::onKey(char c){
	const float trans = 0.1f;
	const float rot = 3 * 3.14f / 180.0f;
	mat3 m = mat3::identity();
	switch (c){
	case 'w':
		m = mat3::translation(0, trans);
		break;
	case 'a':
		m = mat3::translation(-trans, 0);
		break;
	case 's':
		m = mat3::translation(0, -trans);
		break;
	case 'd':
		m = mat3::translation(trans, 0);
		break;
	case '+':
		m = mat3::rotation(rot);
		break;
	case '-':
		m = mat3::rotation(-rot);
		break;
	}
	for (int i = 0; i < 3; i++)
		triB[i] = m * triB[i];
}


TriangleScene::TriangleScene(){
	triA = vec2(-1, -1);
	triB = vec2(0, 1);
	triC = vec2(1, -1);
	//idle();
	const float OFF = 4;

	for (int i = 0; i < sizeof(points) / sizeof(points[0]); i++)
		points[i] = vec2(random() * OFF - OFF / 2.0f, random() * OFF - OFF / 2.0f);
	glPointSize(3);
}

void TriangleScene::render(float delta){
	glBegin(GL_TRIANGLES);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex2fv(triA.data());
	glVertex2fv(triB.data());
	glVertex2fv(triC.data());
	glEnd();

	glBegin(GL_POINTS);
	for (int i = 0; i < sizeof(points) / sizeof(points[0]); i++){
		TriangleLocalization l = findInTriangle(points[i], triA, triB, triC);
		glColor3f(l & 4, l & 2, l & 1);
		glVertex2fv(points[i].data());
	}
	glEnd();
}

ClosestPairScene::ClosestPairScene(){
	glMatrixMode(GL_PROJECTION);
	//glOrtho(0, 200, 0, 200, -1, 1);

	randomize();
	glPointSize(5);
}

void ClosestPairScene::render(float delta){
	glColor3f(1, 1, 1);

	glBegin(GL_POINTS);
	const int pointCount = sizeof(points) / sizeof(vec2);
	for (int i = 0; i < pointCount; i++){
		bool belongsToClosest = i == closetsPair.first || i == closetsPair.second;
		glColor3f(1, !belongsToClosest, !belongsToClosest);
		glVertex2fv(points[i].data());
	}
	glEnd();
}

void ClosestPairScene::randomize(){
	const int pointCount = sizeof(points) / sizeof(vec2);
	for (int i = 0; i < pointCount; i++){
		points[i] = vec2(random() * 3.8f - 2, random() * 3.8f - 2);
	}
	closetsPair = findClosestPair(points, pointCount);
}

void ClosestPairScene::onKey(char c){
	if (c == 'r')
		randomize();
}


DivideAndConquerTriangulationScene::DivideAndConquerTriangulationScene() : InteractiveScene(400){	}

void DivideAndConquerTriangulationScene::render(float delta){
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (points.size() > 2){
		glBegin(GL_TRIANGLES);
		glColor3f(1, 1, 1);
		for (int i = 0; i < triangles.size(); i++)
			glVertex2fv(points[triangles[i]].data());
		glEnd();
	}
	else {
		glBegin(getDrawPrimitive());
		glColor3f(1, 0, 0);
		for (vec2 p : points)
			glVertex2fv(p.data());
		glEnd();
	}
	

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0);
	for (int i = 0; i < triangles.size(); i++)
		glVertex2fv(points[triangles[i]].data());
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

GLenum DivideAndConquerTriangulationScene::getDrawPrimitive(){
	int pointCount = points.size();
	return pointCount > 2 ? GL_TRIANGLES : pointCount > 1 ? GL_LINE_STRIP : GL_POINTS;
}

void DivideAndConquerTriangulationScene::onPointAdded(){
	triangles = divideAndConquerTriangulate(points.data(), points.size());
}


EarClippingTriangulationScene::EarClippingTriangulationScene() : DivideAndConquerTriangulationScene(){	}

void EarClippingTriangulationScene::onPointAdded(){
	//triangles = earClippingTriangulate(points.data(), points.size());
	triangles = divideAndConquerTriangulate(points.data(), points.size());
}


IncrementalTriangulationScene::IncrementalTriangulationScene() : DivideAndConquerTriangulationScene(){
	glPointSize(5);
}

void IncrementalTriangulationScene::render(float delta){
	glBegin(getDrawPrimitive());
	glBegin(GL_TRIANGLES);
	glColor3fv(getDrawColor());
	for (int i = 0; i < triangles.size(); i++)
		glVertex2fv(points[triangles[i]].data());
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0);
	for (int i = 0; i < triangles.size(); i++)
		glVertex2fv(points[triangles[i]].data());
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(0, 1, 0);
	glBegin(GL_POINTS);
	for (int i = 0; i < points.size(); i++){
		vec2 v = points[i];
		glVertex2fv(v.data());
	}
	glEnd();
}

GLenum IncrementalTriangulationScene::getDrawPrimitive(){
	int pointCount = points.size();
	return pointCount > 2 ? GL_POLYGON : pointCount > 1 ? GL_LINE_STRIP : GL_POINTS;
}

void IncrementalTriangulationScene::onPointAdded(){
	triangles = incrementalTriangulate(points.data(), points.size());
}


ConvexPolygonScene::ConvexPolygonScene() : InteractiveScene(400), isConvex(true){	}

const GLfloat* ConvexPolygonScene::getDrawColor(){
	return successColors[isConvex];
}

GLenum ConvexPolygonScene::getDrawPrimitive(){
	int pointCount = points.size();
	return pointCount > 2 ? GL_POLYGON : pointCount > 1 ? GL_LINE_STRIP : GL_POINTS;
}

void ConvexPolygonScene::onPointAdded(){
	isConvex = convex(points.data(), points.size());
}




static const char triNames[3][20] {"Incremetal", "Divisão e conquista", "Ear clipping"};
MetricsScene::MetricsScene() : DivideAndConquerTriangulationScene(), funcIndex(0){
	glPointSize(5);
	funcs[0] = incrementalTriangulate;
	funcs[1] = divideAndConquerTriangulate;
	funcs[2] = earClippingTriangulate;
}

void MetricsScene::render(float delta){
	glBegin(getDrawPrimitive());
	glBegin(GL_TRIANGLES);
	glColor3fv(getDrawColor());
	for (int i = 0; i < triangles.size(); i++)
		glVertex2fv(points[triangles[i]].data());
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0);
	for (int i = 0; i < triangles.size(); i++)
		glVertex2fv(points[triangles[i]].data());
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(0, 1, 0);
	glBegin(GL_POINTS);
	for (int i = 0; i < points.size(); i++){
		vec2 v = points[i];
		glVertex2fv(v.data());
	}
	glEnd();

	drawGUI();
}

void MetricsScene::drawGUI(){
	char temp1[100] = "";
	strcat(temp1, "Triangulação: ");
	strcat(temp1, triNames[funcIndex]);
	glColor3f(1, 1, 0);
	glRasterPos3f(30.0f, 385.0f, 0.0f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char*)temp1);

	if (triangles.size() > 0){
		glColor3f(1, 1, 0);
		char tmp[100] = "";
		sprintf(tmp, "Aspect Ratio\nMin: %.2f, Max: %.2f, Avg: %.2f", ratioMin, ratioMax, ratioAvg);
		glRasterPos3f(30.0f, 85.0f, 0.0f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char*)tmp);
		tmp[0] = '\0';
		sprintf(tmp, "Skewness\nMin: %.2f, Max: %.2f, Avg: %.2f", skMin, skMax, skAvg);
		glRasterPos3f(30.0f, 55.0f, 0.0f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char*)tmp);
		tmp[0] = '\0';
		sprintf(tmp, "Smoothness\nMin: %.2f, Max: %.2f, Avg: %.2f", smMin, smMax, smAvg);
		glRasterPos3f(30.0f, 25.0f, 0.0f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char*)tmp);
	}
}


void MetricsScene::onPointAdded(){
	triangles = funcs[funcIndex](points.data(), points.size());
	if (triangles.size() > 0){
		getAspectRatio(points.data(), triangles, ratioMin, ratioMax, ratioAvg);
		getSkewness(points.data(), triangles, skMin, skMax, skAvg);
		getSmoothness(points.data(), triangles, smMin, smMax, smAvg);
	}
}

void MetricsScene::onKey(char c) {
	if (c == '+')
		funcIndex = (funcIndex + 1) % 3;
	if (c == '-'){
		funcIndex--;
		if (funcIndex < 0)
			funcIndex = 2;
	}

	if (c == '+' || c == '-')
		onPointAdded();
}