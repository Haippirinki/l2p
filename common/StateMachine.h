#pragma once

#include <map>
#include <string>

class State;

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	void update(int width, int height, double t, double dt);
	void render(int width, int height);

	void addState(const std::string& name, State* state);
	void requestState(const std::string& name);

	double getTime() const { return m_t; }
	double getDeltaTime() const { return m_dt; }

	int getWindowWidth() const { return m_windowWidth; }
	int getWindowHeight() const { return m_windowHeight; }

	int getFramebufferWidth() const { return m_framebufferWidth; }
	int getFramebufferHeight() const { return m_framebufferHeight; }

	void mouseDown(float x, float y);

private:
	std::map<std::string, State*> m_state;
	std::string m_requestedState;
	State* m_activeState;
	double m_t;
	double m_dt;
	int m_windowWidth;
	int m_windowHeight;
	int m_framebufferWidth;
	int m_framebufferHeight;
};
