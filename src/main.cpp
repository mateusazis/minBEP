#include "../include/Scenes.h"
#include "../include/Input.h"

#include <GL/freeglut.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <clocale>
#include <deque>

static Scene *s;
static clock_t startT, endT;
static float timeDiff;
const int WINDOW_SIZE = 400;

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

	std::deque<vec2> points;
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
	glutPassiveMotionFunc(onMouseMove);
	glutMouseFunc(onMouse);

	glClearColor(0, 0, 0, 1);

	
	float hSize = WINDOW_SIZE / 2.0f;
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, WINDOW_SIZE, 0, WINDOW_SIZE, -hSize, hSize);
	glMatrixMode(GL_MODELVIEW);
	s = new MinBEPScene();
	glutMainLoop();
	return 0;
}