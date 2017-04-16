#include "StateMachine.h"
#include "State.h"

StateMachine::StateMachine() : m_activeState(nullptr), m_t(0.0), m_dt(0.0), m_windowWidth(0.f), m_windowHeight(0.f), m_framebufferWidth(0), m_framebufferHeight(0)
{
}

StateMachine::~StateMachine()
{
}

void StateMachine::update(float width, float height, double t, double dt)
{
	m_windowWidth = width;
	m_windowHeight = height;
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

void StateMachine::render(int width, int height)
{
	m_framebufferWidth = width;
	m_framebufferHeight = height;
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
