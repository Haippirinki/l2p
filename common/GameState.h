#pragma once

#include "State.h"

class GameState : public State
{
public:
	void enter(StateMachine* stateMachine) override;
	void leave(StateMachine* stateMachine) override;

	void update(StateMachine* stateMachine) override;
	void render(StateMachine* stateMachine) override;

	void mouseDown(StateMachine* stateMachine, float x, float y) override;

private:
};
