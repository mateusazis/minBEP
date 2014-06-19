#pragma once

class Scene
{
public:
	Scene();
	virtual ~Scene();
	virtual void render();
	virtual void onKey(char c);
	virtual void onMouseDown();
};