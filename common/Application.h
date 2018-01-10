#pragma once

#include "StateMachine.h"

#include <cstdint>

namespace Render
{
	class Device;
	class SwapChain;
}

class Application
{
public:
	Application();
	~Application();

	void update(float width, float height);
	void render(int width, int height);

	void mouseDown(float x, float y);
	void mouseUp(float x, float y);
	void mouseMove(float x, float y);

private:
	bool m_initialized;
	Render::Device* m_device;
	Render::SwapChain* m_swapChain;
	StateMachine* m_stateMachine;
	int64_t m_startTime;
	int64_t m_lastUpdateTime;
};
