#include "Application.h"
#include "OpenGL.h"

#include "GameState.h"
#include "MenuState.h"
#include "TestState.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
static int64_t s_frequency = 0;
#else
#include <sys/time.h>
#endif

static int64_t getMicroseconds()
{
#ifdef _WIN32
	int64_t counter;
	QueryPerformanceCounter((LARGE_INTEGER*)&counter);
	return counter * 1000 / (s_frequency / 1000);
#else
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000L + tv.tv_usec;
#endif
}

Application::Application() : m_initialized(false)
{
#ifdef _WIN32
	QueryPerformanceFrequency((LARGE_INTEGER*)&s_frequency);
#endif
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
