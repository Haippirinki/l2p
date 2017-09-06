#include "TestState.h"
#include "StateMachine.h"

#include "Render/Device.h"

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

void TestState::render(StateMachine* stateMachine, Render::Device* device, const Render::RenderTarget* renderTarget)
{
	device->setViewport(0, 0, renderTarget->getWidth(), renderTarget->getHeight());
	device->clearRenderTargetColor(0.f, 0.f, 0.f, 1.f);
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
