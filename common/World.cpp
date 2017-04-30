#include "World.h"
#include "Batcher.h"

#include <cmath>
#include <list>

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
		batcher.addCircle(m_position, m_radius, m_color);
	}

	vec2 m_position;
	float m_speed;
	float m_radius;
	vec4 m_color;
};

struct World::PrivateData
{
	vec2 playerPosition;
	vec2 playerVelocity;
	vec2 playerControl;
	float playerRadius;

	std::list<Enemy> enemies;
};

World::World() : m(new PrivateData)
{
	m->playerPosition = vec2::zero;
	m->playerVelocity = vec2::zero;
	m->playerControl = vec2::zero;
	m->playerRadius = 0.05f;

	for(int i = 0; i < 300; i++)
	{
		float t = float(i);
		float r = 3.f + t * 0.25f;
		float a = 3.7f * t;
		float s = 1.f + 0.5f * sinf(t);
		m->enemies.push_back({ { cosf(a) * r, sinf(a) * r }, s, 0.05f,{ 0.f, s - 0.5f, 1.f, 1.f } });
	}
}

World::~World()
{
	delete m;
}

bool World::update(float dt)
{
	m->playerVelocity = m->playerControl;
	m->playerPosition += m->playerVelocity * dt;

	for(std::list<Enemy>::iterator it = m->enemies.begin(); it != m->enemies.end();)
	{
		if(!it->update(dt))
		{
			it = m->enemies.erase(it);
		}
		else
		{
			if(length(it->m_position - m->playerPosition) < it->m_radius + m->playerRadius)
			{
				return false;
			}

			++it;
		}
	}

	return true;
}

void World::render(Batcher& batcher) const
{
	batcher.addCircle(m->playerPosition, 0.05f, { 1.f, 0.f, 0.f, 1.f });

	for(std::list<Enemy>::const_iterator it = m->enemies.begin(); it != m->enemies.end(); ++it)
	{
		it->render(batcher);
	}
}

void World::setControl(const vec2& control)
{
	m->playerControl = control;
}
