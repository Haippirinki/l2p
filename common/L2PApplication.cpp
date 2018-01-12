#include "L2PApplication.h"

#include "GameState.h"
#include "MenuState.h"
#include "TestState.h"

#include "StateMachine.h"
#include "Render/Device.h"

#include "OpenGL.h"

struct L2PApplication::PrivateData
{
	bool initialized;
	Render::Device* device;
	Render::SwapChain* swapChain;
	StateMachine* stateMachine;
	int64_t startTime;
	int64_t lastUpdateTime;
};

L2PApplication::L2PApplication() : m(new PrivateData)
{
	m->device = nullptr;
	m->swapChain = nullptr;
	m->stateMachine = nullptr;
}

L2PApplication::~L2PApplication()
{
	delete m->stateMachine;
	delete m->swapChain;
	delete m->device;

	delete m;
}

void L2PApplication::init()
{
	m->device = new Render::Device;
	m->swapChain = m->device->createSwapChain();
	m->stateMachine = new StateMachine;

	m->stateMachine->addState("game", new GameState(m->device));
	m->stateMachine->addState("menu", new MenuState(m->device));
	m->stateMachine->addState("test", new TestState);
	m->stateMachine->requestState("menu");
}

void L2PApplication::update(float width, float height, double t, double dt)
{
	m->stateMachine->update(width, height, t, dt);
}

void L2PApplication::render(int width, int height)
{
	GLint framebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer);

	m->device->updateSwapChain(m->swapChain, framebuffer, width, height);

	m->stateMachine->render(m->device, m->swapChain->getBackBuffer());

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void L2PApplication::mouseDown(float x, float y)
{
	m->stateMachine->mouseDown(x, y);
}

void L2PApplication::mouseUp(float x, float y)
{
	m->stateMachine->mouseUp(x, y);
}

void L2PApplication::mouseMove(float x, float y)
{
	m->stateMachine->mouseMove(x, y);
}

Application* createApplication()
{
	return new L2PApplication;
}
