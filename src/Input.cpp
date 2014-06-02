#include "../include/Input.h"
#include <set>

vec2 Input::mousePos(0, 0);
static std::set<char> pressedKeys;

vec2 Input::mousePosition(){
	return mousePos;
}

void Input::updateMouse(int x, int y){
	mousePos.x() = x;
	mousePos.y() = y;
}

void Input::addKey(char c){
	pressedKeys.insert(c);
}

void Input::removeKey(char c){
	pressedKeys.erase(pressedKeys.find(c));
}

bool Input::checkKey(char c){
	return pressedKeys.find(c) != pressedKeys.end();
}