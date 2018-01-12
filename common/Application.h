#pragma once

class Application
{
public:
	virtual ~Application();

	virtual void init() = 0;
	virtual void update(float width, float height, double t, double dt) = 0;
	virtual void render(int width, int height) = 0;

	virtual void mouseDown(float x, float y) = 0;
	virtual void mouseUp(float x, float y) = 0;
	virtual void mouseMove(float x, float y) = 0;
};

Application* createApplication();
