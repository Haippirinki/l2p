#include "GameState.h"
#include "OpenGL.h"
#include "StateMachine.h"

void GameState::enter(StateMachine* stateMachine)
{
}

void GameState::leave(StateMachine* stateMachine)
{
}

void GameState::update(StateMachine* stateMachine)
{
}

void GameState::render(StateMachine* stateMachine)
{
	glClearColor(0.f, 0.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void GameState::mouseDown(StateMachine* stateMachine, float x, float y)
{
	stateMachine->requestState("menu");
}
