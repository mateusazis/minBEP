#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include "angel.h"

class PointSorter{
public:
	static std::vector<int> byX(vec2 *points, int count);
	static std::vector<int> byY(vec2 *points, int count);
	static std::vector<int> byPolarAngle(vec2 *points, int count, vec2 center, int ignored = -1);
};

float triangleArea(vec2 a, vec2 b, vec2 c);

#endif