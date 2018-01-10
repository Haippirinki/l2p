#include "StateMachine.h"
#include "State.h"
#include "Render/Device.h"

#include <map>
#include <string>

struct StateMachine::PrivateData
{
	std::map<std::string, State*> state;
	std::string requestedState;
	State* activeState;
	double t;
	double dt;
	float windowWidth;
	float windowHeight;
	int framebufferWidth;
	int framebufferHeight;
};

StateMachine::StateMachine() : m(new PrivateData)
{
	m->activeState = nullptr;
	m->t = 0.0;
	m->dt = 0.0;
	m->windowWidth = 0.f;
	m->windowHeight = 0.f;
	m->framebufferWidth = 0;
	m->framebufferHeight = 0;
}

StateMachine::~StateMachine()
{
	for(std::map<std::string, State*>::const_iterator it = m->state.begin(); it != m->state.end(); ++it)
	{
		delete it->second;
	}

	delete m;
}

void StateMachine::update(float width, float height, double t, double dt)
{
	m->windowWidth = width;
	m->windowHeight = height;
	m->t = t;
	m->dt = dt;

	if(!m->requestedState.empty())
	{
		if(m->activeState)
		{
			m->activeState->leave(this);
		}

		m->activeState = m->state[m->requestedState];
		m->activeState->enter(this);
		m->requestedState.clear();
	}

	m->activeState->update(this);
}

void StateMachine::render(Render::Device* device, const Render::RenderTarget* renderTarget)
{
	m->activeState->render(this, device, renderTarget);
}

void StateMachine::addState(const char* name, State* state)
{
	m->state[name] = state;
}

void StateMachine::requestState(const char* name)
{
	m->requestedState = name;
}

double StateMachine::getTime() const
{
	return m->t;
}

double StateMachine::getDeltaTime() const
{
	return m->dt;
}

float StateMachine::getWindowWidth() const
{
	return m->windowWidth;
}

float StateMachine::getWindowHeight() const
{
	return m->windowHeight;
}

void StateMachine::mouseDown(float x, float y)
{
	if(m->activeState)
	{
		m->activeState->mouseDown(this, x, y);
	}
}

void StateMachine::mouseUp(float x, float y)
{
	if(m->activeState)
	{
		m->activeState->mouseUp(this, x, y);
	}
}

void StateMachine::mouseMove(float x, float y)
{
	if(m->activeState)
	{
		m->activeState->mouseMove(this, x, y);
	}
}
