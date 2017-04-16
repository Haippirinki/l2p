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
	void render();

	void addState(const std::string& name, State* state);
	void requestState(const std::string& name);

	double getTime() const { return m_t; }
	double getDeltaTime() const { return m_dt; }

	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }

	void mouseDown(float x, float y);

private:
	std::map<std::string, State*> m_state;
	std::string m_requestedState;
	State* m_activeState;
	double m_t;
	double m_dt;
	int m_width;
	int m_height;
};
