#pragma once

namespace Render
{
	class Device;
	class RenderTarget;
}

class State;

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	void update(float width, float height, double t, double dt);
	void render(Render::Device* device, const Render::RenderTarget* renderTarget);

	void addState(const char* name, State* state);
	void requestState(const char* name);

	double getTime() const;
	double getDeltaTime() const;

	float getWindowWidth() const;
	float getWindowHeight() const;

	void mouseDown(float x, float y);
	void mouseUp(float x, float y);
	void mouseMove(float x, float y);

private:
	struct PrivateData;
	PrivateData* m;
};
