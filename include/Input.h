#pragma once
#include <utility>
#include "../include/angel.h"

class Input{
public:
	static vec2 mousePosition();
	static void updateMouse(int x, int y);
	static void addKey(char c);
	static void removeKey(char c);
	static bool checkKey(char c);
	static bool checkKeyDown(char c);

private:
	static vec2 mousePos;
};