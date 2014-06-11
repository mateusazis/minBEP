#pragma once

#include "SP.h"
#include "Scenes.h"

class MyScene : public EarClippingTriangulationScene {
public:
	MyScene();
	void render(float delta);
	vec2 getCenter(int triangleIndex);
	void onPointAdded();
	void onKey(char c);
	void onMouseDown();

private:
	void saveToFile();
	void loadFromFile();
	Graph dualGraph;
	std::vector<vec2> testPoints;
	std::vector<int> sp;
	std::deque<std::deque<int>> funnels;
	int currFunnel = 0;
};