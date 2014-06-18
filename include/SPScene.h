#pragma once

#include "SP.h"
#include "Scene.h"

class MyScene : public Scene {
public:
	MyScene();

	//Callbacks overrides
	void render(float delta);
	void onKey(char c);
	void onMouseDown();

	void resetPath();
private:
	void onPointAdded();
	vec2 getCenter(int triangleIndex);

	void drawPolygon();
	void drawGraph();
	void drawDFSTree();
	void drawFunnels();
	void drawSP();
	void drawVertexNumbers();

	void saveToFile();
	void loadFromFile();
	Graph dualGraph;
	std::vector<vec2> testPoints, points;
	std::vector<int> sp, triangles;
	std::deque<std::deque<int>> funnels;
	int currFunnel = 0;

	int showTriangulation, showGraph, showTree, showFunnels, showNumbers;
};