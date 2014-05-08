#include "../include/Input.h"

vec2 Input::mousePos(0,0);

vec2 Input::mousePosition(){
	return mousePos;
}

void Input::updateMouse(int x, int y){
	mousePos.x() = x;
	mousePos.y() = y;
}