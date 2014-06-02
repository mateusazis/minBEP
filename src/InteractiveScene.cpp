#include "../include/InteractiveScene.h"
#include "../include/Input.h"

InteractiveScene::InteractiveScene(int windowSize){
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
	//if (points.size() < 3)
		points.push_back(Input::mousePosition());
	/*else
		points.insert(points.end() - 2, Input::mousePosition());*/
	
	onPointAdded();
}

void InteractiveScene::onKey(char c){
	if (c == 'r'){
		points.clear();
	}
}

static const GLfloat white[3] = { 1, 1, 1 };

const GLfloat* InteractiveScene::getDrawColor(){
	return &white[0];
}

GLenum InteractiveScene::getDrawPrimitive(){
	return GL_POINTS;
}