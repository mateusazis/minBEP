#pragma once
#include "../include/Scene.h"
#include "../include/angel.h"
#include <GL/freeglut.h>
#include <vector>


/*
Cena que adiciona um ponto automaticamente a cada clique do usuário.
Subclasses podem variar a forma como os pontos são desenhados.
*/
class InteractiveScene : public Scene {
public:
	InteractiveScene();
	virtual void render(float delta);
	void onMouseDown();
	virtual void onKey(char c);

	virtual void onPointAdded() = 0;
	virtual const GLfloat* getDrawColor();
	virtual GLenum getDrawPrimitive();
protected:
	std::vector<vec2> points;
};