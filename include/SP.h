#include "../include/angel.h"
#include "../include/GeneralProblems.h"
#include <vector>
#pragma once

#include <deque>
#include <set>
#include <stack>

typedef std::vector<std::set<int>> Graph;

class Diagonal {
public:
	Diagonal(int first, int second);
	int a, b;
};

std::vector<int> DepthFirstSearch(vec2 src, vec2 target, const vec2* points, const Graph & g, const std::vector<int> & triangles);

Graph getDualGraph(const std::vector<int> & triangulation);

std::vector<int> SP(vec2 src, vec2 dest, const vec2* points, int count, std::deque<std::deque<int>> & funnels);