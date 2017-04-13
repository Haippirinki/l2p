#pragma once

class Application
{
public:
	Application();
	~Application();

	void update(int width, int height);

	void mouseDown(float x, float y);
};
