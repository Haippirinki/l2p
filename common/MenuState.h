#pragma once

#include "State.h"

class MenuState : public State
{
public:
	MenuState(Render::Device* device);
	~MenuState();

	void enter(StateMachine* stateMachine) override;
	void leave(StateMachine* stateMachine) override;

	void update(StateMachine* stateMachine) override;
	void render(StateMachine* stateMachine, Render::Device* device, const Render::RenderTarget* renderTarget) override;

	void mouseDown(StateMachine* stateMachine, float x, float y) override;
	void mouseUp(StateMachine* stateMachine, float x, float y) override;
	void mouseMove(StateMachine* stateMachine, float x, float y) override;

private:
	struct PrivateData;
	PrivateData* m;
};
