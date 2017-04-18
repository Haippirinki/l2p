#include "Application.h"
#include "OpenGL.h"

#include "GameState.h"
#include "MenuState.h"
#include "TestState.h"

Application::Application() : m_initialized(false)
{
}

Application::~Application()
{
}

void Application::update(float width, float height)
{
	if(!m_initialized)
	{
		m_stateMachine.addState("game", new GameState);
		m_stateMachine.addState("menu", new MenuState);
		m_stateMachine.addState("test", new TestState);
		m_stateMachine.requestState("game");
		m_initialized = true;
	}

	m_stateMachine.update(width, height, 0.0, 0.0);
}

void Application::render(int width, int height)
{
	m_stateMachine.render(width, height);
}

void Application::mouseDown(float x, float y)
{
	m_stateMachine.mouseDown(x, y);
}
