#include "InteractiveScene.h"
#include "Input.h"

InteractiveScene::InteractiveScene(int windowSize){
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, windowSize / 2.0f, 0, windowSize / 2.0f, -1, 1);
}

void InteractiveScene::render(float delta){
		int pointCount = points.size();

		glBegin(getDrawPrimitive());
		glColor3fv(getDrawColor());
		for (int i = 0; i < pointCount; i++){
			glVertex2fv(points[i].data());
		}
		glEnd();
	}

void InteractiveScene::onMouseDown(){
	points.push_back(Input::mousePosition());
	onPointAdded();
}

static const GLfloat white[3] = { 1, 1, 1 };

const GLfloat* InteractiveScene::getDrawColor(){
	return &white[0];
}

GLenum InteractiveScene::getDrawPrimitive(){
	return GL_POINTS;
}