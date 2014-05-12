#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <vector>
#include "../include/GeneralProblems.h"
#include <GL/freeglut.h>
#include <ctime>
#include "../include/InteractiveScene.h"
#include "../include/lista1.h"
#include "../include/Input.h"
#include <clocale>

using namespace std;
Scene *s;
int mouseX, mouseY;
const GLfloat successColors[2][3] = {
	1,0,0,
	0,1,0
};

void hullTest(){
	vec2 p[] = {
		vec2(1, 1),
		vec2(-1, 1),
		vec2(-0.3f, 0.7f),
		vec2(-0.3f, -0.7f),
		vec2(-1, -1),
		vec2(0.3f, -0.7f),
		vec2(1, -1),
		vec2(0.3f, 0.7f),
	};
	vector<vec2> hull = getHull(p, sizeof(p) / sizeof(p[0]));
	for (int i = 0; i < hull.size(); i++){
		vec2 v = hull[i];
		printf("%.1f %.1f\n", v.x(), v.y());
	}
}

void pointLocationTest(){
	vec2 a = vec2(1, 1);
	vec2 b = vec2(7, 7);
	vec2 testPoints[] = {
		vec2(10, 1),
		vec2(0, 1),
		vec2(0, 0),
		vec2(1, 1),
		vec2(2, 2),
		vec2(7, 7),
		vec2(8, 8),
	};
	for (int i = 0; i < 7; i++)
		printf("%d\n", findInSegment(testPoints[i], a, b));
}

class ConvexityScene : public Scene{
public:
	ConvexityScene(){
		
		glOrtho(-SIZE, SIZE, -SIZE, SIZE, -1, 1);
		u = vec2(2, 1);
		v = vec2(1, 2);
	}

	void render(float delta){
		glBegin(GL_LINES);

		glColor3f(1, 1, 1);
		glVertex2f(0, 0);
		glVertex2fv(u.data());
		glVertex2f(0, 0);
		glVertex2fv(v.data());
		w = vec2((mouseX - 200) / 100.0f * SIZE, (mouseY - 200) / 100.0f * SIZE);
		Convexidade c = getConvexity(u, v, w);
		glColor3f(c == NAO, c == SIM, 0);
		glVertex2f(0, 0);
		glVertex2fv(w.data());
		glEnd();
	}

	void onKey(char c){
		const float OFFSET = 3 * 3.14f / 180.0f;
		if (c == '+')
			v = mat3::rotation(OFFSET) * v;
		if (c == '-')
			v = mat3::rotation(-OFFSET) * v;
	}

	const float SIZE = 2;
	vec2 u, v, w;
};

class DisjointsTriangles : public Scene{
public:
	DisjointsTriangles(){
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

	void render(float delta){
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

	void onKey(char c){
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
	const float SIZE = 2;
	vec2 triA[3], triB[3];
};

class TriangleScene : public Scene{
public:
	TriangleScene(){
		triA = vec2(-1, -1);
		triB = vec2(0, 1);
		triC = vec2(1, -1);
		//idle();
		const float OFF = 4;

		for (int i = 0; i < sizeof(points) / sizeof(points[0]); i++)
			points[i] = vec2(random() * OFF - OFF / 2.0f, random() * OFF - OFF / 2.0f);
		glPointSize(3);
	}

	void render(float delta){
		const float colors[] = {
			1, 0, 0,
			1, 0, 0,
			1, 0, 0,
			1, 0, 0,
			1, 0, 0,
			1, 0, 0,
			1, 0, 0,
			1, 0, 0,
			0, 1, 0
		};
	

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

	vec2 triA, triB, triC;
	vec2 points[1000];
	float angle = 0;
	mat3 rot;
};

class ClosestPairScene : public Scene{
public:
	ClosestPairScene(){
		glMatrixMode(GL_PROJECTION);
		//glOrtho(0, 200, 0, 200, -1, 1);
		
		randomize();
		glPointSize(5);
	}

	void render(float delta){
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

	void randomize(){
		const int pointCount = sizeof(points) / sizeof(vec2);
		for (int i = 0; i < pointCount; i++){
			points[i] = vec2(random() * 3.8f - 2, random() * 3.8f - 2);
		}
		closetsPair = findClosestPair(points, pointCount);
	}

	void onKey(char c){
		if (c == 'r')
			randomize();
	}

	vec2 points[11];
	pair<int, int> closetsPair;
};

class DivideAndConquerTriangulationScene : public InteractiveScene{
public:
	DivideAndConquerTriangulationScene() : InteractiveScene(400){	}

	void render(float delta){
		InteractiveScene::render(delta);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);
		for (int i = 0; i < triangles.size(); i++)
			glVertex2fv(points[triangles[i]].data());
		glEnd();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	GLenum getDrawPrimitive(){
		int pointCount = points.size();
		return pointCount > 2 ? GL_POLYGON : pointCount > 1 ? GL_LINE_STRIP : GL_POINTS;
	}

	void onPointAdded(){
		triangles = divideAndConquerTriangulate(points.data(), points.size());
	}

	vector<int> triangles;
};

class EarClippingTriangulationScene : public DivideAndConquerTriangulationScene{
public:
	EarClippingTriangulationScene() : DivideAndConquerTriangulationScene(){	}

	void onPointAdded(){
		triangles = earClippingTriangulate(points.data(), points.size());
	}
};

class IncrementalTriangulationScene : public DivideAndConquerTriangulationScene{
public:
	IncrementalTriangulationScene() : DivideAndConquerTriangulationScene(){	
		glPointSize(5);
	}

	void render(float delta){


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

	GLenum getDrawPrimitive(){
		int pointCount = points.size();
		return pointCount > 2 ? GL_POLYGON : pointCount > 1 ? GL_LINE_STRIP : GL_POINTS;
	}

	void onPointAdded(){
		triangles = incrementalTriangulate(points.data(), points.size());
	}
};

class ConvexPolygonScene : public InteractiveScene{
public:
	ConvexPolygonScene() : InteractiveScene(400), isConvex(true){	}

	const GLfloat* getDrawColor(){
		return successColors[isConvex];
	}

	GLenum getDrawPrimitive(){
		int pointCount = points.size();
		return pointCount > 2 ? GL_POLYGON : pointCount > 1 ? GL_LINE_STRIP : GL_POINTS;
	}

	void onPointAdded(){
		isConvex = convex(points.data(), points.size());
	}

	bool isConvex;
};



float timeDiff;


void display(){
	glClear(GL_COLOR_BUFFER_BIT);

	s->render(timeDiff);

	glutSwapBuffers();
}

clock_t startT, endT;

void idle(){
	endT = clock();
	timeDiff = (endT - startT) / ((float)CLOCKS_PER_SEC);

	startT = endT;

	display();
}

void onKeyboard(unsigned char c, int x, int y){
	const unsigned char ESC = 27;
	if (c == ESC)
		exit(0);
	else{
		s->onKey(c);
	}
}

void onMouse(int button, int pressed, int x, int y){
	if (pressed == 1)
		s->onMouseDown();
}

void onMouse2(int x, int y){
	mouseX = x;
	mouseY = 400-y;
	Input::updateMouse(mouseX, mouseY);
}

int getQuestionNumber(){
	printf("Escolha um exercício a executar:\n");
	printf("1) Primitivas Geométricas - Questão 4 (ângulo convexo)\n");
	printf("2) Polígonos - Questão 1 (polígono convexo)\n");
	printf("3) Polígonos - Questão 2 (triangulação por divisão e conquista)\n");
	printf("4) Polígonos - Questão 4 (triangulação por corte de orelhas)\n");
	printf("5) Problemas fundamentais - Questão 1 (localização em triângulo)\n");
	printf("6) Problemas fundamentais - Questão 2 (triângulos disjuntos)\n");
	printf("7) Problemas fundamentais - Questão 3 (par mais próximo)\n");
	printf("8) Problemas fundamentais - Questão 4 (triangulação incremental)\n");
	int resp;
	scanf("%d", &resp);
	return resp;
}

void setupExercises(){
	s = new IncrementalTriangulationScene();
	return;
	int question = getQuestionNumber();
	switch (question){
	case 1:
		s = new ConvexityScene();
		break;
	case 2:
		s = new ConvexPolygonScene();
		break;
	case 3:
		s = new DivideAndConquerTriangulationScene();
		break;
	case 4:
		s = new EarClippingTriangulationScene(); //trocar para ear cutting
		break;
	case 5:
		s = new TriangleScene();
		break;
	case 6:
		s = new DisjointsTriangles();
		break;
	case 7:
		s = new ClosestPairScene();
		break;
	case 8:
		s = new IncrementalTriangulationScene();
		break;
	}
}

int main(int argc, char **argv){
	setlocale(LC_ALL, "Portuguese");
	startT = endT = clock();
	srand(time(NULL));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(400, 400);
	
	glutCreateWindow("Lista 1 - Computação Gemérica");
	glutHideWindow();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(onKeyboard);
	glutPassiveMotionFunc(onMouse2);
	glutMouseFunc(onMouse);
	
	setupExercises();

	glutShowWindow();

	glClearColor(0, 0, 0, 1);
	glOrtho(-2, 2, -2, 2, -2, 2);
	glutMainLoop();
	return 0;
}