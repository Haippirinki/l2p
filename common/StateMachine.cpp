#include "StateMachine.h"
#include "State.h"

StateMachine::StateMachine() : m_activeState(nullptr)
{
}

StateMachine::~StateMachine()
{
}

void StateMachine::update(double t, double dt)
{
	m_t = t;
	m_dt = dt;

	if(!m_requestedState.empty())
	{
		if(m_activeState)
		{
			m_activeState->leave(this);
		}

		m_activeState = m_state[m_requestedState];
		m_activeState->enter(this);
		m_requestedState.clear();
	}

	m_activeState->update(this);
}

void StateMachine::render()
{
	m_activeState->render(this);
}

void StateMachine::addState(const std::string& name, State* state)
{
	m_state[name] = state;
}

void StateMachine::requestState(const std::string& name)
{
	m_requestedState = name;
}

void StateMachine::mouseDown(float x, float y)
{
	if(m_activeState)
	{
		m_activeState->mouseDown(this, x, y);
	}
}
