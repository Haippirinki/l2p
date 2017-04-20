#include "Application.h"
#include "OpenGL.h"

#include "GameState.h"
#include "MenuState.h"
#include "TestState.h"

#include <sys/time.h>

static int64_t getMicroseconds()
{
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000L + tv.tv_usec;
}

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

		m_startTime = m_lastUpdateTime = getMicroseconds();

		m_initialized = true;
	}

	int64_t time = getMicroseconds();

	m_stateMachine.update(width, height, (time - m_startTime) * (1.0 / 1000000.0), (time - m_lastUpdateTime) * (1.0 / 1000000.0));

	m_lastUpdateTime = time;
}

void Application::render(int width, int height)
{
	m_stateMachine.render(width, height);
}

void Application::mouseDown(float x, float y)
{
	m_stateMachine.mouseDown(x, y);
}

void Application::mouseUp(float x, float y)
{
	m_stateMachine.mouseUp(x, y);
}

void Application::mouseMove(float x, float y)
{
	m_stateMachine.mouseMove(x, y);
}
