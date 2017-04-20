#pragma once

#include "StateMachine.h"

class Application
{
public:
	Application();
	~Application();

	void update(float width, float height);
	void render(int width, int height);

	void mouseDown(float x, float y);
	void mouseUp(float x, float y);
	void mouseMove(float x, float y);

private:
	bool m_initialized;
	StateMachine m_stateMachine;
};
