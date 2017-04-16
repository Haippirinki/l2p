#include "Application.h"
#include "OpenGL.h"

#include "GameState.h"
#include "MenuState.h"

Application::Application()
{
	m_stateMachine.addState("game", new GameState);
	m_stateMachine.addState("menu", new MenuState);
	m_stateMachine.requestState("menu");
}

Application::~Application()
{
}

void Application::update(int width, int height)
{
	m_stateMachine.update(width, height, 0.0, 0.0);
	m_stateMachine.render();
}

void Application::mouseDown(float x, float y)
{
	m_stateMachine.mouseDown(x, y);
}
