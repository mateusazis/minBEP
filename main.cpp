#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <vector>
#include "GeneralProblems.h"
#include <GL/freeglut.h>
#include <ctime>
#include "Scene.h"
#include "lista1.h"

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
	vector<vec2>* hull = getHull(p, sizeof(p) / sizeof(p[0]));
	for (int i = 0; i < hull->size(); i++){
		vec2 v = hull->operator[](i);
		printf("%.1f %.1f\n", v.x(), v.y());
	}
	delete hull;
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

		const float PI = 3.14f;
		//angle += 0.0000001f;
		const float SPEED = PI;
		angle += delta * PI;
		if (angle >= PI * 2)
			angle -= PI * 2;
		rot = mat3::rotation(angle);
		vec2 triAT = rot * triA;
		vec2 triBT = rot * triB;
		vec2 triCT = rot * triC;

		

		glBegin(GL_TRIANGLES);
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex2fv(triAT.data());
		glVertex2fv(triBT.data());
		glVertex2fv(triCT.data());
		glEnd();

		glBegin(GL_POINTS);
		for (int i = 0; i < sizeof(points) / sizeof(points[0]); i++){
			TriangleLocalization l = findInTriangle(points[i], triAT, triBT, triCT);
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

class PolygonScene : public Scene{
public:
	PolygonScene(){
		glMatrixMode(GL_PROJECTION);
		glOrtho(0, 200, 0, 200, -1, 1);
		vec2 p[] = {
			vec2(10,10),
			vec2(20, 300),
			//vec2(200, 350),
			vec2(100, 350),
			vec2(200, 100),
			vec2(300, 350),
			vec2(380, 300),
			vec2(380, 20),
		};
		pointCount = sizeof(p) / sizeof(vec2);
		points = new vec2[pointCount];
		memcpy(points, p, sizeof(p));
		glPointSize(20);

		b = new unsigned char[400 * 400 * 3];
		
	}

	void render(float delta){
		glColor3f(1, 1, 1);

		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < pointCount; i++){
			glVertex2fv(points[i].data());
		}
		glEnd();

		glBegin(GL_POINTS);
		bool inside = findInPolygon(vec2(mouseX, mouseY), points, pointCount);
		glColor3f(!inside, inside, 0);
		glVertex2f(mouseX, mouseY);
		glEnd();

		glReadPixels(0, 0, 400, 400, GL_RGB, GL_UNSIGNED_BYTE, b);
		for (int i = 0; i < 400; i++)
		for (int j = 0; j < 400; j++){
			int base = (i * 400 + j) * 3;
			if (b[base] == 0 && b[base + 1] == 0 && b[base + 2] == 0){
				bool found = findInPolygon(vec2(j, i), points, pointCount);
				b[base] = !found * 255;
				b[base + 1] = found * 255;
			}
		}
		glRasterPos2f(0, 0);
		glDrawPixels(400, 400, GL_RGB, GL_UNSIGNED_BYTE, b);
	}

	vec2 *points;
	int pointCount;
	unsigned char *b;
};

class ClosestPairScene : public Scene{
public:
	ClosestPairScene(){
		glMatrixMode(GL_PROJECTION);
		glOrtho(0, 200, 0, 200, -1, 1);
		
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
			points[i] = vec2(random() * 200, random() * 200);
		}
		closetsPair = findClosestPair(points, pointCount);
	}

	void onKey(char c){
		if (c == 'a'){
			randomize();
		}
	}

	vec2 points[11];
	pair<int, int> closetsPair;
};

class TriangulationScene : public Scene{
public:
	TriangulationScene(){
		glMatrixMode(GL_PROJECTION);
		glOrtho(0, 200, 0, 200, -1, 1);
		//randomize();
	}

	void render(float delta){
		glColor3f(1, 1, 1);
		int pointCount = points.size();
		GLenum mode = pointCount > 2 ? GL_POLYGON : pointCount > 1 ? GL_LINE_STRIP : GL_POINTS;

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(mode);
		glColor3f(1, 1, 1);
		for (int i = 0; i < pointCount; i++){
			glVertex2fv(points[i].data());
		}
		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);
		for (int i = 0; i < triangles.size(); i++){
			glVertex2fv(points[triangles[i]].data());
		}
		glEnd();
	}

	void onMouseDown(){
		points.push_back(vec2((mouseX), mouseY));
		triangles = incrementalTriangulate(points.data(), points.size());
	}

	vector<vec2> points;
	vector<int> triangles;
};

class ConvexPolygonScene : public Scene{
public:
	ConvexPolygonScene(){
		glMatrixMode(GL_PROJECTION);
		glOrtho(0, 200, 0, 200, -1, 1);
		//randomize();
	}

	void render(float delta){
		glColor3f(1, 1, 1);
		int pointCount = points.size();
		GLenum mode = pointCount > 2 ? GL_POLYGON : pointCount > 1 ? GL_LINE_STRIP : GL_POINTS;

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(mode);

		if (mode == GL_POLYGON)
			glColor3fv(successColors[isConvex]);
		else
			glColor3f(1, 1, 1);
		for (int i = 0; i < pointCount; i++){
			glVertex2fv(points[i].data());
		}
		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void onMouseDown(){
		points.push_back(vec2((mouseX), mouseY));
		isConvex = convex(points.data(), points.size());
	}

	vector<vec2> points;
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
}

void triangleLocationTest(){
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(400, 400);
	glutCreateWindow("Triangle Localization");
	


	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(onKeyboard);
	glutPassiveMotionFunc(onMouse2);
	glutMouseFunc(onMouse);
	

	glClearColor(0, 0, 0, 1);
	glOrtho(-2, 2, -2, 2, -2, 2);
	
	//s = new TriangleScene();
	//s = new PolygonScene();
	//s = new ConvexityScene();
	//s = new DisjointsTriangles();
	//s = new ClosestPairScene();
	/*s = new TriangulationScene();*/
	s = new ConvexPolygonScene();
	
	glutMainLoop();
}

int main(int argc, char **argv){
	startT = endT = clock();

	glutInit(&argc, argv);
	srand(time(NULL));
	//hullTest();
	//pointLocationTest();
	triangleLocationTest();
	return 0;
}