#pragma once

#include <map>
#include <string>

class State;

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	void update(double t, double dt);
	void render();

	void addState(const std::string& name, State* state);
	void requestState(const std::string& name);

	void mouseDown(float x, float y);

private:
	std::map<std::string, State*> m_state;
	std::string m_requestedState;
	State* m_activeState;
	double m_t;
	double m_dt;
};
