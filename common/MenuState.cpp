#include "MenuState.h"
#include "OpenGL.h"
#include "StateMachine.h"

void MenuState::enter(StateMachine* stateMachine)
{
}

void MenuState::leave(StateMachine* stateMachine)
{
}

void MenuState::update(StateMachine* stateMachine)
{
}

void MenuState::render(StateMachine* stateMachine)
{
	glClearColor(1.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void MenuState::mouseDown(StateMachine* stateMachine, float x, float y)
{
	stateMachine->requestState("game");
}

void MenuState::mouseUp(StateMachine* stateMachine, float x, float y)
{
}

void MenuState::mouseMove(StateMachine* stateMachine, float x, float y)
{
}
