#include "../include/Input.h"
#include <set>

vec2 Input::mousePos(0, 0);
static std::set<char> pressedKeys, justPressedKeys;

vec2 Input::mousePosition(){
	return mousePos;
}

void Input::updateMouse(int x, int y){
	mousePos.x() = x;
	mousePos.y() = y;
}

void Input::addKey(char c){
	pressedKeys.insert(c);
	std::set<char>::const_iterator it = justPressedKeys.find(c);
	if (it == justPressedKeys.end())
		justPressedKeys.insert(c);
	else
		justPressedKeys.erase(it);
}

void Input::removeKey(char c){
	pressedKeys.erase(c);
	std::set<char>::const_iterator it = justPressedKeys.find(c);
	if (it != justPressedKeys.end())
		justPressedKeys.erase(it);
}

bool Input::checkKey(char c){
	return pressedKeys.find(c) != pressedKeys.end();
}

bool Input::checkKeyDown(char c){
	return justPressedKeys.find(c) != justPressedKeys.end();
}