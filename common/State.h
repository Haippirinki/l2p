#pragma once

class StateMachine;

class State
{
public:
	virtual ~State();

	virtual void enter(StateMachine* stateMachine) = 0;
	virtual void leave(StateMachine* stateMachine) = 0;

	virtual void update(StateMachine* stateMachine) = 0;
	virtual void render(StateMachine* stateMachine) = 0;

	virtual void mouseDown(StateMachine* stateMachine, float x, float y) = 0;
	virtual void mouseUp(StateMachine* stateMachine, float x, float y) = 0;
	virtual void mouseMove(StateMachine* stateMachine, float x, float y) = 0;
};
