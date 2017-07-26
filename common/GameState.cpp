#include "GameState.h"
#include "Batcher.h"
#include "DDSLoader.h"
#include "File.h"
#include "Math.h"
#include "OpenGL.h"
#include "Profile.h"
#include "StateMachine.h"
#include "World.h"

static vec2 windowToView(StateMachine* stateMachine, const vec2& p)
{
	const float ww = stateMachine->getWindowWidth();
	const float wh = stateMachine->getWindowHeight();
	const float ws = (ww > wh) ? wh : ww;
	return { (2.f * p.x - ww) / ws, (wh - 2.f * p.y) / ws };
}

static vec2 viewToWindow(StateMachine* stateMachine, const vec2& p)
{
	const float ww = stateMachine->getWindowWidth();
	const float wh = stateMachine->getWindowHeight();
	const float ws = (ww > wh) ? wh : ww;
	return { 0.5f * (p.x * ws + ww), 0.5f * (wh - p.y * ws) };
}

struct GameState::PrivateData
{
	GLuint shaderProgram;
	GLuint whiteTexture;
	Batcher batcher;

	float joystickAreaRadius;
	float joystickStickRadius;
	float joystickMaxOffset;

	bool joystickActive;
	vec2 joystickCenter;
	vec2 joystickPosition;

	World* world;

	double exitTime;
};

GameState::GameState() : m(new PrivateData)
{
	File vsFile("assets/shaders/game.vs");
	GLuint vs = createShader(GL_VERTEX_SHADER, vsFile.getData(), vsFile.getSize());

	File fsFile("assets/shaders/game.fs");
	GLuint fs = createShader(GL_FRAGMENT_SHADER, fsFile.getData(), fsFile.getSize());

	m->shaderProgram = createProgram(vs, fs);
	glUseProgram(m->shaderProgram);
	glUniform1i(glGetUniformLocation(m->shaderProgram, "u_sampler"), 0);

	size_t width, height, depth;
	GLenum bindTarget;

	File whiteTextureFile("assets/images/white.dds");
	m->whiteTexture = loadDDS(whiteTextureFile.getData(), whiteTextureFile.getSize(), true, width, height, depth, bindTarget);

	m->joystickAreaRadius = 0.2f;
	m->joystickStickRadius = 0.1f;
	m->joystickMaxOffset = 0.1f;

	m->world = nullptr;
}

GameState::~GameState()
{
	delete m;
}

void GameState::enter(StateMachine* stateMachine)
{
	m->joystickActive = false;

	File levelFile(Profile::getLevelName(Profile::getCurrentLevel()).c_str());

	m->world = new World;
	m->world->init(levelFile.getData(), levelFile.getSize());

	m->exitTime = 0.0;
}

void GameState::leave(StateMachine* stateMachine)
{
	delete m->world;
	m->world = nullptr;
}

void GameState::update(StateMachine* stateMachine)
{
	if(!m->exitTime)
	{
		if(m->joystickActive)
		{
			vec2 d = windowToView(stateMachine, m->joystickPosition) - windowToView(stateMachine, m->joystickCenter);
			m->world->setControl(d / m->joystickMaxOffset);
		}
		else
		{
			m->world->setControl(vec2::zero);
		}

		m->world->update(stateMachine->getTime(), (float)stateMachine->getDeltaTime());

		if(m->world->getState() == World::Lost)
		{
			m->exitTime = stateMachine->getTime() + 0.5;
		}
		else if(m->world->getState() == World::Won)
		{
			if(!Profile::getLevelName(Profile::getCurrentLevel() + 1).empty())
			{
				Profile::setCurrentLevel(Profile::getCurrentLevel() + 1);
			}
			else
			{
				Profile::setCurrentLevel(0);
			}
			m->exitTime = stateMachine->getTime() + 0.5;
		}
	}
}

void GameState::render(StateMachine* stateMachine)
{
	glViewport(0, 0, stateMachine->getFramebufferWidth(), stateMachine->getFramebufferHeight());
	switch(m->world->getState())
	{
	case World::Playing:
		glClearColor(1.f, 0.9406f, 0.7969f, 1.f);
		break;

	case World::Lost:
		glClearColor(1.f, 0.5f, 0.5f, 1.f);
		break;

	case World::Won:
		glClearColor(0.5f, 1.f, 0.5f, 1.f);
		break;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(m->shaderProgram);

	float sx, sy;
	if(stateMachine->getFramebufferWidth() > stateMachine->getFramebufferHeight())
	{
		sx = float(stateMachine->getFramebufferHeight()) / float(stateMachine->getFramebufferWidth());
		sy = 1.f;
	}
	else
	{
		sx = 1.f;
		sy = float(stateMachine->getFramebufferWidth()) / float(stateMachine->getFramebufferHeight());
	}

	float mvp[] = {
		sx, 0.f, 0.f, 0.f,
		0.f, sy, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	glUniformMatrix4fv(glGetUniformLocation(m->shaderProgram, "u_modelViewProjection"), 1, GL_FALSE, mvp);

	glBindTexture(GL_TEXTURE_2D, m->whiteTexture);

	m->world->render(stateMachine->getTime(), m->batcher);

	if(m->joystickActive)
	{
		vec2 c = windowToView(stateMachine, m->joystickCenter);
		vec2 p = windowToView(stateMachine, m->joystickPosition);

		m->batcher.addCircle(c, m->joystickAreaRadius, { 0.5f, 0.5f, 0.5f, 0.333f } );
		m->batcher.addCircle(p, m->joystickStickRadius, { 0.5f, 0.5f, 0.5f, 0.333f } );
	}

	m->batcher.flush();
}

void GameState::mouseDown(StateMachine* stateMachine, float x, float y)
{
	if(m->exitTime && m->exitTime <= stateMachine->getTime())
	{
		stateMachine->requestState("menu");
	}
	else
	{
		m->joystickActive = true;
		m->joystickCenter = m->joystickPosition = { x, y };
	}
}

void GameState::mouseUp(StateMachine* stateMachine, float x, float y)
{
	m->joystickActive = false;
}

void GameState::mouseMove(StateMachine* stateMachine, float x, float y)
{
	m->joystickPosition = { x, y };

	if(m->joystickActive)
	{
		vec2 c = windowToView(stateMachine, m->joystickCenter);
		vec2 p = windowToView(stateMachine, m->joystickPosition);
		if(length(p - c) > m->joystickMaxOffset)
		{
			m->joystickCenter = viewToWindow(stateMachine, p + normalize(c - p) * m->joystickMaxOffset);
		}
	}
}
