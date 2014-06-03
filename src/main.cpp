#include "../include/Scenes.h"
#include "../include/Input.h"

#include <GL/freeglut.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <clocale>
#include <deque>
#include "../include/lista1.h"
#include "../include/GeneralProblems.h"
#include "../include/angel.h"
#include <algorithm>
#include <set>
#include <stack>

static Scene *s;
static clock_t startT, endT;
static float timeDiff;
const int WINDOW_SIZE = 400;

using namespace std;

/* Callbacks da Freeglut */
void display(){
	glClear(GL_COLOR_BUFFER_BIT);
	s->render(timeDiff);
	glutSwapBuffers();
}

void idle(){
	endT = clock();
	timeDiff = (endT - startT) / ((float)CLOCKS_PER_SEC);

	startT = endT;

	display();
}

void onKeyboard(unsigned char c, int x, int y){
	Input::addKey(c);
	const unsigned char ESC = 27;
	if (c == ESC)
		exit(0);
	else
		s->onKey(c);
}

void onKeyboardUp(unsigned char c, int x, int y){
	Input::removeKey(c);
	const unsigned char ESC = 27;
	if (c == ESC)
		exit(0);
	else
		s->onKey(c);
}

void onMouse(int button, int pressed, int x, int y){
	if (pressed == 1)
		s->onMouseDown();
}

void onMouseMove(int x, int y){
	int mouseX = x;
	int mouseY = WINDOW_SIZE - y;
	Input::updateMouse(mouseX, mouseY);
}

class Funnel {
public:
	Funnel(){}
	Funnel(int startVertex, int adjacentVertex){
		vertices.push_back(adjacentVertex);
		vertices.push_back(startVertex);
		cuspIndex = 1;
	}

	int getTangent(int end, const std::vector<vec2> points){
		//search for a such that a-end does not belong to the frustum
		
		/*for (int i = 0; i < points.size(); i++){
			for (int j = 0; j < vertices.size() - 1; j++){
				if (!segmentIntersects(points[i], points[end], points[vertices[j]], points[vertices[j + 1]]))
					return i;
			}
		}*/

		bool foundIntersection = false;
		for (int j = 0; j < vertices.size() - 1; j++){
			if (j != cuspIndex && j != cuspIndex + 1){
				if (segmentIntersects(points[vertices[cuspIndex]], points[end], points[vertices[j]], points[vertices[j + 1]]))
				{
					foundIntersection = true;
					break;
				}
			}
		}
		if (!foundIntersection){
			printf("ret case 1\n");
			return vertices[cuspIndex];
		}


		int bestIndex = 0;
		float bestAngle = 1;
		vec2 vEnd = points[end];
		for (int i = 0; i < cuspIndex; i++){
			vec2 v = points[vertices[i]];
			vec2 next = points[vertices[i+1]];
			vec2 diff1 = (v - next).normalized(),
				diff2 = (vEnd - next).normalized();
			float dot = abs(diff1.dot(diff2));
			if (dot < bestAngle){
				bestAngle = dot;
				bestIndex = vertices[i];
			}
		}

		for (int i = vertices.size() - 1; i > cuspIndex; i--){
			vec2 v = points[vertices[i]];
			vec2 next = points[vertices[i - 1]];
			vec2 diff1 = (v - next).normalized(),
				diff2 = (vEnd - next).normalized();
			float dot = abs(diff1.dot(diff2));
			if (dot < bestAngle){
				bestAngle = dot;
				bestIndex = vertices[i];
			}
		}

		return bestIndex;
	}
	static Funnel forward(std::deque<int>::iterator it, int v, int end){
		Funnel resp;
		while (*it != v){
			resp.vertices.push_back(*it);
			it++;
		}
		resp.vertices.push_back(v);
		resp.vertices.push_back(end);
		resp.cuspIndex = resp.vertices.size() - 1;

		return resp;
	}

	static Funnel back(std::deque<int>::reverse_iterator it, int v, int end){
		Funnel resp;
		resp.vertices.push_back(end);
		resp.vertices.push_back(v);
		resp.cuspIndex = 0;

		while (*it != v){
			resp.vertices.push_back(*it);
			it++;
		}

		return resp;
	}


	int cuspIndex;
	std::deque<int> vertices;
};

class S2 : public DivideAndConquerTriangulationScene {
public:
	S2() : DivideAndConquerTriangulationScene(), startIndex(0), endIndex(0){
		glPointSize(10);
	}

	void render(float delta){
		DivideAndConquerTriangulationScene::render(delta);

		if (points.size() > 0){
			glBegin(GL_POINTS);
			glColor3f(0, 1, 0);
			glVertex2fv(points[startIndex].data());
			glColor3f(1, 1, 0);
			glVertex2fv(points[endIndex].data());
			glEnd();
		}

		if (SP.size() >= 2){
			glColor3f(0, 0, 1);
			glBegin(GL_LINE_STRIP);
			for (std::vector<int>::iterator it = SP.begin(); it < SP.end(); it++){
				glVertex2fv(points[*it].data());
			}
			glEnd();
		}
	}

	void onPointAdded(){
		DivideAndConquerTriangulationScene::onPointAdded();
		printf("POints: %d\n", points.size());
	}

	void onKey(char c){
		DivideAndConquerTriangulationScene::onKey(c);
		switch (c){
		case 'a':
			startIndex = (startIndex + 1) % points.size();
			break;
		case 'd':
			startIndex = (startIndex + points.size() - 1) % points.size();
			break;
		case 'w':
			endIndex = (endIndex + 1) % points.size();
			break;
		case 's':
			endIndex = (endIndex + points.size() - 1) % points.size();
			break;
		}

		if (c == 'c')
			calc();
	}

	void calc(){
		int adjacentVertex = (startIndex + 1) % points.size();
		Funnel f = Funnel(startIndex, adjacentVertex);
		std::vector<int> triangles = divideAndConquerTriangulate(points.data(), points.size());
		Path(f, triangles, startIndex, endIndex);
	}

	void Path(Funnel f, const std::vector<int> & tris, int start, int end){
		int u = f.vertices[0];
		int w = *(f.vertices.end() - 1);

		int v = f.getTangent(end, points);

		//splitting
		Funnel f1 = Funnel::forward(f.vertices.begin(), v, end);
		Funnel f2 = Funnel::back(f.vertices.rbegin(), v, end);

		SP.clear();
		SP.push_back(startIndex);
		SP.push_back(v);
		SP.push_back(endIndex);
	}

	int startIndex, endIndex;

	std::vector<int> SP;
};

class MinBEPScene : public Scene {
	void render(float delta){
		if (points.size() >= 2){
			float w = 20;
			for (int i = 0; i < points.size() - 1; i++)
				drawFirstCorridor(points[i], points[i + 1], w);
			for (int i = 1; i < points.size() - 1; i++)
				drawJoint(points[i-1], points[i], points[i + 1], w);
		}
	}

	vec2 getNormal(vec2 a, vec2 b){
		vec2 diff = (b - a).normalized();
		return mat3::rotation(3.14f / 2.0f) * diff;
	}

	std::pair<float, float> getLineCoefs(vec2 a, vec2 b){
		float c1 = (b.y() - a.y()) / (b.x() - a.x());
		float c2 = a.y() - a.x() * c1;
		return std::pair<float, float>(c1, c2);
	}

	vec2 findIntersection(vec2 srcA, vec2 dirA, vec2 srcB, vec2 dirB){
		std::pair<float, float> c1 = getLineCoefs(srcA, srcA + dirA),
			c2 = getLineCoefs(srcB, srcB + dirB);

		float x = (c1.second - c2.second) / (c2.first - c1.first);
		float y = c1.first * x + c1.second;
		return vec2(x, y);
	}

	void drawJoint(vec2 a, vec2 b, vec2 c, float width){
		vec2 normalAB = getNormal(a, b),
			normalBC = getNormal(b, c);
		float hWidth = width / 2.0f;

		float sign = (a - b).crossSign(c - b);
		vec2 v1 = b + normalAB * hWidth * sign,
			v2 = b + normalBC * hWidth * sign;
		vec2 vertices[] = {
			b,
			v1,
			findIntersection(v1, b - a, v2, c - b),
			v2
		};
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		for (int i = 0; i < 4; i++)
			glVertex2fv(vertices[i].data());
		glEnd();
	}

	void drawFirstCorridor(vec2 a, vec2 b, float width){
		vec2 normal = getNormal(a, b);
		float hWidth = width / 2.0f;
		vec2 vertices[] = {
			a + normal * hWidth,
			a - normal * hWidth,
			b - normal * hWidth,
			b + normal * hWidth
		};
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		for (int i = 0; i < 4; i++)
			glVertex2fv(vertices[i].data());
		glEnd();
	}

	void onMouseDown(){
		points.push_back(Input::mousePosition());
	}

	void onKey(char c){
		if (c == 'r')
			points.clear();
	}

	std::deque<vec2> points;
};

typedef vector<set<int>> Graph;

int findTriangle(vec2 v, vec2* points, vector<int> & triangles){
	int i;
	for (i = 0; i < triangles.size() / 3; i++){
		vector<int>::iterator start = triangles.begin() + i * 3;
		TriangleLocalization loc = findInTriangle(v,	points[start[0]], points[start[1]], points[start[2]]);
		if (loc == INSIDE)
			return i;
	}
	return -1;
}

static void pushPath(int src, int dest, const int *parent, vector<int> & out){
	if (src != dest)
		pushPath(src, parent[dest], parent, out);
	out.push_back(dest);
}

vector<int> DepthFirstSearch(vec2 src, vec2 target, vec2* points, Graph & g, vector<int> & triangles){
	//find first triangle
	vector<int> resp;
	int srcTriangle = findTriangle(src, points, triangles);
	int destTriangle = findTriangle(target, points, triangles);
	
	if (destTriangle == srcTriangle){
		resp.push_back(srcTriangle);
	}
	else{
		set<int> visited;
		stack<int> toVisit;
		toVisit.push(srcTriangle);

		int triCount = triangles.size() / 3;
		int *parent = new int[triCount];
		std::fill_n(parent, triCount, -1);
		int t;

		while (toVisit.size() > 0 && (t = toVisit.top()) != destTriangle){
			toVisit.pop();

			for (int dest : g[t])
			if (visited.find(dest) == visited.end()){
				parent[dest] = t;
				toVisit.push(dest);
			}
				
			visited.insert(t);
		}

		pushPath(srcTriangle, destTriangle, parent, resp);
		delete parent;
	}
	return resp;
}

Graph getDualGraph(vector<int> & triangulation){
	Graph resp(triangulation.size() / 3);
	if (triangulation.size() > 0){
		int out[3];
		for (int i = 0; i < triangulation.size() - 3; i += 3){
			int a[3] = { triangulation[i], triangulation[i + 1], triangulation[i + 2] };
			sort(a, a + 3);
			for (int j = i + 3; j < triangulation.size(); j += 3){
				int b[3] = { triangulation[j], triangulation[j + 1], triangulation[j + 2] };
				sort(b, b + 3);
				if (set_intersection(a, a + 3, b, b + 3, out) - out >= 2){
					resp[i / 3].insert(j / 3);
					resp[j / 3].insert(i / 3);
				}
			}
		}
	}
	
	return resp;
}

class MyScene : public EarClippingTriangulationScene {
public:
	void render(float delta){
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
			glBegin(GL_POINTS);
			glColor3f(0, 1, 0);
			glVertex2fv(testPoints[0].data());
			glVertex2fv(testPoints[1].data());
			glEnd();

			vector<int> path = DepthFirstSearch(testPoints[0], testPoints[1], points.data(), dualGraph, triangles);
			glBegin(GL_LINE_STRIP);
			for (int t : path){
				glVertex2fv(getCenter(t).data());
			}
			glEnd();
		}
		
	}

	vec2 getCenter(int triangleIndex){
		triangleIndex *= 3;
		return (points[triangles[triangleIndex]] + points[triangles[triangleIndex + 1]] + points[triangles[triangleIndex + 2]]) / 3.0f;
	}

	void onPointAdded(){
		EarClippingTriangulationScene::onPointAdded();
		dualGraph = getDualGraph(triangles);
	}

	void onMouseDown(){
		
		if (Input::checkKey('d')){
			testPoints.push_back(Input::mousePosition());
			printf("add\n");
		}
		else {
			EarClippingTriangulationScene::onMouseDown();
		}
	}

private:
	Graph dualGraph;
	vector<vec2> testPoints;
};

int main(int argc, char **argv){
	setlocale(LC_ALL, "Portuguese");
	startT = endT = clock();
	srand(time(NULL));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
	
	glutCreateWindow("Lista 1 - Geometria Computacional");
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutPassiveMotionFunc(onMouseMove);
	glutMouseFunc(onMouse);

	glClearColor(0, 0, 0, 1);

	
	float hSize = WINDOW_SIZE / 2.0f;
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, WINDOW_SIZE, 0, WINDOW_SIZE, -hSize, hSize);
	glMatrixMode(GL_MODELVIEW);
	//s = new S2();
	s = new MyScene();
	glutMainLoop();
	return 0;
}