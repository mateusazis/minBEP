#include "../include/Input.h"
#include "../include/lista1.h"
#include "../include/GeneralProblems.h"
#include "../include/SPScene.h"

#include <GL/freeglut.h>
#include <GL/glui.h>

#include <ctime>
#include <clocale>


static Scene *s;
static clock_t startT, endT;
static float timeDiff;
const int WINDOW_SIZE = 400;
int mainWindowID;
using namespace std;

/* INÍCIO - Callbacks da Freeglut */
void display(){
	glClear(GL_COLOR_BUFFER_BIT);
	s->render(timeDiff);
	glutSwapBuffers();
}

void idle(){
	endT = clock();
	timeDiff = (endT - startT) / ((float)CLOCKS_PER_SEC);

	startT = endT;

	//display();
	glutSetWindow(mainWindowID);
	glutPostRedisplay();
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
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);


	float aspectW = (float)WINDOW_SIZE / tw;
	float aspecth = (float)WINDOW_SIZE / th;
	int mouseX = (x - tx) * aspectW;
	int mouseY = (WINDOW_SIZE - y - ty) * aspecth;
	Input::updateMouse(mouseX, mouseY);
}

void onReshape(int w, int h){
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);
}
/* FIM - Callbacks da Freeglut */



int main(int argc, char **argv){
	setlocale(LC_ALL, "Portuguese");
	startT = endT = clock();
	srand((unsigned int)time(NULL));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_SIZE+200, WINDOW_SIZE);
	
	mainWindowID = glutCreateWindow("Shortest Path");
	glutDisplayFunc(display);
	//glutIdleFunc(idle);
	GLUI_Master.set_glutIdleFunc(idle);
	GLUI_Master.set_glutKeyboardFunc(onKeyboard);
	GLUI_Master.set_glutMouseFunc(onMouse);
	GLUI_Master.set_glutReshapeFunc(onReshape);
	//glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutPassiveMotionFunc(onMouseMove);
	//glutMouseFunc(onMouse);

	glClearColor(0, 0, 0, 1);

	
	float hSize = WINDOW_SIZE / 2.0f;
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, WINDOW_SIZE, 0, WINDOW_SIZE, -hSize, hSize);
	glMatrixMode(GL_MODELVIEW);
	s = new MyScene();

	glutMainLoop();
	return 0;
}