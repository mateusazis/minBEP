#ifndef SCENES_H
#define SCENES_H

#include "InteractiveScene.h"
#include "angel.h"
#include <vector>
#include <utility>

class ConvexityScene : public Scene{
public:
	ConvexityScene();

	void render(float delta);

	void onKey(char c);

	const float SIZE = 2;
	vec2 u, v;
};

class DisjointsTriangles : public Scene{
public:
	DisjointsTriangles();
	void render(float delta);
	void onKey(char c);

	const float SIZE = 2;
	vec2 triA[3], triB[3];
};

class TriangleScene : public Scene{
public:
	TriangleScene();
	void render(float delta);

	vec2 triA, triB, triC;
	vec2 points[1000];
	float angle = 0;
	mat3 rot;
};

class ClosestPairScene : public Scene{
public:
	ClosestPairScene();
	void render(float delta);
	void randomize();
	void onKey(char c);

	vec2 points[11];
	std::pair<int, int> closetsPair;
};

class DivideAndConquerTriangulationScene : public InteractiveScene{
public:
	DivideAndConquerTriangulationScene();
	void render(float delta);
	GLenum getDrawPrimitive();
	void onPointAdded();

	std::vector<int> triangles;
};

class EarClippingTriangulationScene : public DivideAndConquerTriangulationScene{
public:
	EarClippingTriangulationScene();
	void onPointAdded();
};

class IncrementalTriangulationScene : public DivideAndConquerTriangulationScene{
public:
	IncrementalTriangulationScene();
	void render(float delta);
	GLenum getDrawPrimitive();
	void onPointAdded();
};

class ConvexPolygonScene : public InteractiveScene{
public:
	ConvexPolygonScene();
	const GLfloat* getDrawColor();
	GLenum getDrawPrimitive();
	void onPointAdded();

	bool isConvex;
};

typedef std::vector<int>(*TriangulationFunction)(vec2 *, int);

class MetricsScene : public DivideAndConquerTriangulationScene{
public:
	MetricsScene();
	void render(float delta);
	void onPointAdded();
	void onKey(char c);
	void drawGUI();

	TriangulationFunction funcs[3];
	int funcIndex;
	float ratioMin, ratioMax, ratioAvg;
	float skMin, skMax, skAvg;
	float smMin, smMax, smAvg;
};

#endif