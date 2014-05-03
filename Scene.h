#pragma once

class Scene
{
public:
	Scene();
	virtual ~Scene();
	virtual void render(float delta);
	virtual void onKey(char c);
	virtual void onMouseDown();
};