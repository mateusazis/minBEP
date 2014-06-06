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


class Diagonal : private pair<int, int> {
public:
	Diagonal(int first, int second) : pair<int, int>(first, second), a(first), b(second){}
	int a, b;
};

static Diagonal getSharedVertices(const int* triA, const int* triB){
	int resp[2];
	int index = 0;
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			if (triA[i] == triB[j]){
				resp[index++] = triA[i];
			}
		}
	}
	return Diagonal(resp[0], resp[1]);
}



int getDifferentVertex(Diagonal d, int* triangle){
	for (int i = 0; i < 3; i++)
		if (triangle[i] != d.a && triangle[i] != d.b)
			return triangle[i];
	return -1;
}


bool isPredecessor(vec2 candidate, vec2 next, vec2 v, bool pastApex){
	vec2 delta = v - candidate, baseSegment;
	/*if (pastApex)
		baseSegment = next - candidate;
	else*/
		baseSegment = candidate - next;
	int cross = baseSegment.crossSign(delta);
	printf("Cross is %d\n", cross);
	return cross == (pastApex ? -1 : 1);
}

static void printFunnel(deque<int> & f){
	printf("[");
	for (int i = 0; i < f.size(); i++){
		printf("%d", f[i]);
		if (i < f.size() - 1)
			printf(", ");
	}
	printf("]\n");
}

bool triangleContainsVertex(int *triangle, int vertex){
	return std::find(triangle, triangle + 3, vertex) != triangle + 3;
}

static deque<int> makeFirstFunnel(vec2 src, vec2* points, int indexA, int indexB){
	vec2 diffA = points[indexA] - src;
	vec2 diffB = points[indexB] - src;
	deque<int> resp;
	if (diffA.crossSign(diffB) >= 0){
		resp.push_back(indexB);
		resp.push_back(-1);
		resp.push_back(indexA);
	}
	else {
		resp.push_back(indexA);
		resp.push_back(-1);
		resp.push_back(indexB);
	}
	return resp;
}

vec2 getFunnelPoint(vec2 src, const vec2* points, deque<int> & funnel, int index){
	if (funnel[index] == -1)
		return src;
	return points[funnel[index]];
}

static int findPredecessor(deque<int> & funnel, int apex, const vec2* points, vec2 src, vec2 dest, int targetIndex, int & relativePos){
	vec2 target = targetIndex == -2 ? dest : points[targetIndex];
	int apexIndex = find(funnel.begin(), funnel.end(), apex) - funnel.begin();
	for (int i = 0; i < apexIndex; i++){
		vec2 a = getFunnelPoint(src, points, funnel, i);
		vec2 b = getFunnelPoint(src, points, funnel, i+1);
		printf("Testing %d as head for %d\n", funnel[i], targetIndex);
		if (isPredecessor(a, b, target, false)){
			relativePos = -1;
			return i;
		}
	}

	/*{
		vec2 a = getFunnelPoint(src, points, funnel, apexIndex);
		vec2 b = getFunnelPoint(src, points, funnel, apexIndex + 1);
		printf("Testing %d as head for %d\n", funnel[apexIndex], targetIndex);
		if (isPredecessor(a, b, target, true)){
			relativePos = 0;
			return apexIndex;
		}
	}*/
	
	for (int i = funnel.size() - 1; i > apexIndex; i--){
		printf("Testing %d as head for %d\n", funnel[i], targetIndex);
		vec2 a = getFunnelPoint(src, points, funnel, i);
		vec2 b = getFunnelPoint(src, points, funnel, i - 1);
		if (isPredecessor(a, b, target, true)){
			relativePos = 1;
			return i;
		}
	}

	/*relativePos = 1;
	return funnel.size() - 1;*/
	relativePos = 0;
	return apexIndex;
}

vector<int> SP(vec2 src, vec2 dest, vec2* points, int count, deque<deque<int>> & funnels){
	vector<int> triangles = divideAndConquerTriangulate(points, count);
	Graph g = getDualGraph(triangles);
	vector<int> tree = DepthFirstSearch(src, dest, points, g, triangles);

	

	vector<int> resp;
	
	if (tree.size() != 1){
		
		int* preds = new int[count];
		std::fill_n(preds, count, -1);
		int nextVertex = -1;

		Diagonal d = getSharedVertices(triangles.data() + tree[0] * 3, triangles.data() + tree[1] * 3);
		preds[d.a] = preds[d.b] = -1;
		deque<int> funnel = makeFirstFunnel(src, points, d.a, d.b);
		//int apexIndex = 1;
		int apex = -1;
		funnels.push_back(funnel);
		for (int i = 1; i < tree.size(); i++){
			
			printf("========== TRIANGLE %d =========\n", i);
			printFunnel(funnel);
			bool lastTriangle = i == tree.size() - 1;
			if (lastTriangle){
				nextVertex = -2;
			}
			else {
				nextVertex = getDifferentVertex(d, triangles.data() + tree[i] * 3);
			}
			
			bool popedApex = false;

			//printf("Testing %d as head for %d\n", head, lastTriangle ? -2 : nextVertex);
			int relativePos;
			int headIndex = findPredecessor(funnel, apex, points, src, dest, nextVertex, relativePos);
			int head = funnel[headIndex];

			printf("chose head as %d\n", head);


			if (!lastTriangle){
				printf("set pred of %d to %d\n", nextVertex, head);
				preds[nextVertex] = head;
				if (popedApex)
					apex = head;

				if (relativePos < 0){
					funnel.erase(funnel.begin(), funnel.begin() + headIndex);
					printf("Pushing left vertex %d\n", nextVertex);
					funnel.push_front(nextVertex);
					funnels.push_back(funnel);
				}
				else if (relativePos > 0){
					funnel.erase(funnel.begin() + headIndex + 1, funnel.end());
					printf("Pushing right vertex %d\n", nextVertex);
					funnel.push_back(nextVertex);
					funnels.push_back(funnel);
				}
				else { //head is the apex
					printf("Next triangle is: %d, %d, %d\n", triangles[tree[i + 1] * 3], triangles[tree[i + 1] * 3 + 1], triangles[tree[i + 1] * 3 + 2]);
					if (triangleContainsVertex(triangles.data() + tree[i + 1] * 3, funnel[0])){
						printf("Triangle %d has vertex %d. Push to the end\n", i+1, funnel[0]);
						funnel.erase(funnel.begin() + headIndex + 1, funnel.end());
						funnel.push_back(nextVertex);
						funnels.push_back(funnel);
					}
					else {
						printf("Triangle %d has NOT vertex %d. Push to the begging\n", i + 1, funnel[0]);
						funnel.erase(funnel.begin(), funnel.begin() + headIndex);
						funnel.push_front(nextVertex);
						funnels.push_back(funnel);
					}
				}
			} else
				nextVertex = head;

			if (!lastTriangle && triangleContainsVertex(triangles.data() + tree[i + 1] * 3, d.a)){
				d.b = nextVertex;
			}
			else {
				d.a = nextVertex;
			}
		}

		

		//nextVertex = preds[nextVertex];
		while (nextVertex != -1){
			printf("pushing vertex %d\n", nextVertex);
			resp.insert(resp.begin(), nextVertex);
			nextVertex = preds[nextVertex];
		}
		delete preds;
	}
	

	return resp;
}



class MyScene : public EarClippingTriangulationScene {
public:
	MyScene() : EarClippingTriangulationScene(){
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

	void render(float delta){
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
				system("cls");
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

			//Draw path
			glLineStipple(1, 0xAAAA);
			glEnable(GL_LINE_STIPPLE);
			glBegin(GL_LINE_STRIP);
			glColor3f(0, 0, 0);
			glVertex2fv(testPoints[0].data());
			for (int i : sp)
				glVertex2fv(points[i].data());
			glVertex2fv(testPoints[1].data());
			glEnd();
			glDisable(GL_LINE_STIPPLE);
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

		
	}

	vec2 getCenter(int triangleIndex){
		triangleIndex *= 3;
		return (points[triangles[triangleIndex]] + points[triangles[triangleIndex + 1]] + points[triangles[triangleIndex + 2]]) / 3.0f;
	}

	void onPointAdded(){
		EarClippingTriangulationScene::onPointAdded();
		dualGraph = getDualGraph(triangles);
		if (testPoints.size() >= 2){
			system("cls");
			sp = SP(testPoints[0], testPoints[1], points.data(), points.size(), funnels);
		}
		
	}

	void onKey(char c){
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

	void saveToFile(){
		FILE* f = fopen("points", "wb");
		int size = points.size();
		fwrite(&size, sizeof(int), 1, f);
		fwrite(points.data(), sizeof(vec2), points.size(), f);
		fclose(f);
	}

	void loadFromFile(){
		FILE* f = fopen("points", "rb");
		int count;
		fread(&count, sizeof(int), 1, f);
		points.resize(count);
		fread(points.data(), sizeof(vec2), count, f);
		fclose(f);
		onPointAdded();
	}

	void onMouseDown(){
		
		if (Input::checkKey('d')){
			testPoints.push_back(Input::mousePosition());
			printf("add\n");
			if (testPoints.size() >= 2){
				system("cls");
				sp = SP(testPoints[0], testPoints[1], points.data(), points.size(), funnels);
			}
		}
		else {
			EarClippingTriangulationScene::onMouseDown();
		}
	}

private:
	Graph dualGraph;
	vector<vec2> testPoints;
	vector<int> sp;
	deque<deque<int>> funnels;
	int currFunnel = 0;
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