#include "Application.h"
#include "OpenGL.h"

#include "GameState.h"
#include "MenuState.h"
#include "TestState.h"

#include "Render/Device.h"

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

Application::Application() : m_initialized(false), m_device(nullptr), m_swapChain(nullptr), m_stateMachine(nullptr)
{
#ifdef _WIN32
	QueryPerformanceFrequency((LARGE_INTEGER*)&s_frequency);
#endif
}

Application::~Application()
{
	delete m_stateMachine;
	delete m_swapChain;
	delete m_device;
}

void Application::update(float width, float height)
{
	if(!m_initialized)
	{
		m_device = new Render::Device;
		m_swapChain = m_device->createSwapChain();
		m_stateMachine = new StateMachine;

		m_stateMachine->addState("game", new GameState(m_device));
		m_stateMachine->addState("menu", new MenuState(m_device));
		m_stateMachine->addState("test", new TestState);
		m_stateMachine->requestState("menu");

		m_startTime = m_lastUpdateTime = getMicroseconds();

		m_initialized = true;
	}

	int64_t time = getMicroseconds();

	m_stateMachine->update(width, height, (time - m_startTime) * (1.0 / 1000000.0), (time - m_lastUpdateTime) * (1.0 / 1000000.0));

	m_lastUpdateTime = time;
}

void Application::render(int width, int height)
{
	GLint framebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer);

	m_device->updateSwapChain(m_swapChain, framebuffer, width, height);

	m_stateMachine->render(m_device, m_swapChain->getBackBuffer());

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void Application::mouseDown(float x, float y)
{
	m_stateMachine->mouseDown(x, y);
}

void Application::mouseUp(float x, float y)
{
	m_stateMachine->mouseUp(x, y);
}

void Application::mouseMove(float x, float y)
{
	m_stateMachine->mouseMove(x, y);
}
