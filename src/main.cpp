#include "../include/Scenes.h"
#include "../include/lista1.h"
#include "../include/Input.h"

#include <GL/freeglut.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <clocale>

Scene *s;
clock_t startT, endT;
float timeDiff;

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
	else{
		s->onKey(c);
	}
}

void onMouse(int button, int pressed, int x, int y){
	if (pressed == 1)
		s->onMouseDown();
}

void onMouseMove(int x, int y){
	int mouseX = x;
	int mouseY = 400-y;
	Input::updateMouse(mouseX, mouseY);
}

int getQuestionNumber(){
	printf("Escolha um exerc�cio a executar:\n");
	printf("1) Primitivas Geom�tricas - Quest�o 4 (�ngulo convexo)\n");
	printf("2) Pol�gonos - Quest�o 1 (pol�gono convexo)\n");
	printf("3) Pol�gonos - Quest�o 2 (triangula��o por divis�o e conquista)\n");
	printf("4) Pol�gonos - Quest�o 4 (triangula��o por corte de orelhas)\n");
	printf("5) Problemas fundamentais - Quest�o 1 (localiza��o em tri�ngulo)\n");
	printf("6) Problemas fundamentais - Quest�o 2 (tri�ngulos disjuntos)\n");
	printf("7) Problemas fundamentais - Quest�o 3 (par mais pr�ximo)\n");
	printf("8) Problemas fundamentais - Quest�o 4 (triangula��o incremental)\n");
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
	
	glutCreateWindow("Lista 1 - Geometria Computacional");
	glutHideWindow();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(onKeyboard);
	glutPassiveMotionFunc(onMouseMove);
	glutMouseFunc(onMouse);
	
	setupExercises();

	glutShowWindow();

	glClearColor(0, 0, 0, 1);
	glOrtho(-2, 2, -2, 2, -2, 2);
	glutMainLoop();
	return 0;
}