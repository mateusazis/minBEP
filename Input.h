#pragma once
#include <utility>
#include "angel.h"

class Input{
public:
	static vec2 mousePosition();
	static void updateMouse(int x, int y);

private:
	static vec2 mousePos;
};