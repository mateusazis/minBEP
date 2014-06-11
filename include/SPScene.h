#pragma once

#include "SP.h"
#include "Scenes.h"

class MyScene : public InteractiveScene {
public:
	MyScene();
	void render(float delta);
	vec2 getCenter(int triangleIndex);
	void onPointAdded();
	void onKey(char c);
	void onMouseDown();

private:
	void drawPolygon();
	void drawGraph();
	void drawDFSTree();
	void drawFunnels();
	void drawSP();
	void drawVertexNumbers();

	void saveToFile();
	void loadFromFile();
	Graph dualGraph;
	std::vector<vec2> testPoints;
	std::vector<int> sp, triangles;
	std::deque<std::deque<int>> funnels;
	int currFunnel = 0;
};