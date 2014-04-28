#pragma once
class Scene
{
public:
	Scene();
	virtual ~Scene();
	virtual void render(float delta);
};

