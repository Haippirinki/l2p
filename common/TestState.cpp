#include "TestState.h"
#include "OpenGL.h"
#include "StateMachine.h"

struct TestState::PrivateData
{
};

TestState::TestState() : m(new PrivateData)
{
}

TestState::~TestState()
{
	delete m;
}

void TestState::enter(StateMachine* stateMachine)
{
}

void TestState::leave(StateMachine* stateMachine)
{
}

void TestState::update(StateMachine* stateMachine)
{
}

void TestState::render(StateMachine* stateMachine)
{
	glViewport(0, 0, stateMachine->getFramebufferWidth(), stateMachine->getFramebufferHeight());
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void TestState::mouseDown(StateMachine* stateMachine, float x, float y)
{
	stateMachine->requestState("menu");
}

void TestState::mouseUp(StateMachine* stateMachine, float x, float y)
{
}

void TestState::mouseMove(StateMachine* stateMachine, float x, float y)
{
}
