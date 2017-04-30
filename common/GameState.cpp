#include "GameState.h"
#include "Batcher.h"
#include "DDSLoader.h"
#include "File.h"
#include "Math.h"
#include "OpenGL.h"
#include "StateMachine.h"

#include <cmath>
#include <cstring>
#include <list>
#include <vector>

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


class Game
{
public:
	Game() : m_playerPosition(vec2::zero), m_playerVelocity(vec2::zero), m_playerControl(vec2::zero), m_playerRadius(0.05f)
	{
		for(int i = 0; i < 300; i++)
		{
			float t = float(i);
			float r = 3.f + t * 0.25f;
			float a = 3.7f * t;
			float s = 1.f + 0.5f * sinf(t);
			m_enemies.push_back( { { cosf(a) * r, sinf(a) * r }, s, 0.05f, { 0.f, s - 0.5f, 1.f, 1.f } } );
		}
	}

	bool update(float dt)
	{
		m_playerVelocity = m_playerControl;
		m_playerPosition += m_playerVelocity * dt;

		for(std::list<Enemy>::iterator it = m_enemies.begin(); it != m_enemies.end();)
		{
			if(!it->update(dt))
			{
				it = m_enemies.erase(it);
			}
			else
			{
				if(length(it->m_position - m_playerPosition) < it->m_radius + m_playerRadius)
				{
					return false;
				}

				++it;
			}
		}

		return true;
	}

	void render(Batcher& batcher) const
	{
		batcher.addCircle(m_playerPosition, 0.05f, { 1.f, 0.f, 0.f, 1.f } );

		for(std::list<Enemy>::const_iterator it = m_enemies.begin(); it != m_enemies.end(); ++it)
		{
			it->render(batcher);
		}
	}

	void setControl(const vec2& control)
	{
		m_playerControl = control;
	}

private:
	vec2 m_playerPosition;
	vec2 m_playerVelocity;
	vec2 m_playerControl;
	float m_playerRadius;

	struct Enemy
	{
		bool update(float dt)
		{
			if(length(m_position) < m_speed * dt)
			{
				return false;
			}

			m_position -= normalize(m_position) * (m_speed * dt);
			return true;
		}

		void render(Batcher& batcher) const
		{
			batcher.addCircle(m_position, m_radius, m_color );
		}

		vec2 m_position;
		float m_speed;
		float m_radius;
		vec4 m_color;
	};

	std::list<Enemy> m_enemies;
};

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

	Game* game;
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

	m->game = nullptr;
}

GameState::~GameState()
{
	delete m;
}

void GameState::enter(StateMachine* stateMachine)
{
	m->joystickActive = false;

	m->game = new Game;
}

void GameState::leave(StateMachine* stateMachine)
{
	delete m->game;
	m->game = nullptr;
}

void GameState::update(StateMachine* stateMachine)
{
	if(m->joystickActive)
	{
		vec2 d = windowToView(stateMachine, m->joystickPosition) - windowToView(stateMachine, m->joystickCenter);
		m->game->setControl(d / m->joystickMaxOffset);
	}
	else
	{
		m->game->setControl(vec2::zero);
	}

	if(!m->game->update((float)stateMachine->getDeltaTime()))
	{
		stateMachine->requestState("menu");
	}
}

void GameState::render(StateMachine* stateMachine)
{
	glViewport(0, 0, stateMachine->getFramebufferWidth(), stateMachine->getFramebufferHeight());
	glClearColor(1.f, 0.9406f, 0.7969f, 1.f);
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

	m->game->render(m->batcher);

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
	m->joystickActive = true;
	m->joystickCenter = m->joystickPosition = { x, y };
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
