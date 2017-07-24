#define _CRT_SECURE_NO_WARNINGS 1
#include "World.h"
#include "Batcher.h"
#include "Util.h"

#include <cassert>
#include <cctype>
#include <cmath>
#include <cstring>
#include <list>

struct Enemy
{
	bool update(float dt)
	{
		if(length(m_position) < m_speed * dt)
		{
			m_position = vec2::zero;
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
	bool m_mayNotTouch;
	bool m_mayNotPass;
};

static bool comparePendingEnemies(const std::pair<float, Enemy>& lhs, const std::pair<float, Enemy>& rhs)
{
	return lhs.first < rhs.first;
}

struct World::PrivateData
{
	State state;

	vec2 playerPosition;
	vec2 playerVelocity;
	vec2 playerControl;
	float playerRadius;

	float time;

	std::list<std::pair<float, Enemy>> pendingEnemies;
	std::list<Enemy> enemies;
	std::list<vec2> playerPositionsList;
};

World::World() : m(new PrivateData)
{
	m->state = Playing;

	m->playerPosition = vec2::zero;
	m->playerVelocity = vec2::zero;
	m->playerControl = vec2::zero;
	m->playerRadius = 0.05f;

	m->time = 0.f;
}

World::~World()
{
	delete m;
}

void World::init(const void* data, size_t size)
{
	std::vector<std::string> lines = getLines((const char*)data, size);
	for(std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it)
	{
		char type[32];
		if(sscanf(it->c_str(), "%s", type) == 1)
		{
			if(strcmp(type, "basic") == 0)
			{
				float time, distance, degrees, speed;
				int n = sscanf(it->c_str() + strlen(type), "%f %f %f %f", &time, &distance, &degrees, &speed);
				assert(n == 4);

				float a = 3.14159f * degrees / 180.f;
				Enemy enemy = { { cosf(a) * distance, sinf(a) * distance }, speed, 0.05f,{ 0.f, speed - 0.5f, 1.f, 1.f }, true, false };
				m->pendingEnemies.push_back(std::pair<float, Enemy>(time, enemy));
			}

			if(strcmp(type, "friendly") == 0)
			{
				float time, distance, degrees, speed;
				int n = sscanf(it->c_str() + strlen(type), "%f %f %f %f", &time, &distance, &degrees, &speed);
				assert(n == 4);

				float a = 3.14159f * degrees / 180.f;
				Enemy enemy = { { cosf(a) * distance, sinf(a) * distance }, speed, 0.05f,{ 1.f, 0.25f, 0.f, 1.f }, false, true };
				m->pendingEnemies.push_back(std::pair<float, Enemy>(time, enemy));
			}
		}
	}

	m->pendingEnemies.sort(comparePendingEnemies);
}

void World::update(float dt)
{
	m->playerVelocity = m->playerControl;
	m->playerPositionsList.push_back(m->playerPosition);
	if(m->playerPositionsList.size() > 10)
	{
		m->playerPositionsList.pop_front();
	}
	m->playerPosition += m->playerVelocity * dt;

	m->time += dt;

	while(!m->pendingEnemies.empty() && m->pendingEnemies.front().first <= m->time)
	{
		m->enemies.push_back(m->pendingEnemies.front().second);
		m->pendingEnemies.pop_front();
	}

	for(std::list<Enemy>::iterator it = m->enemies.begin(); it != m->enemies.end();)
	{
		if(!it->update(dt))
		{
			if(it->m_mayNotPass)
			{
				m->state = Lost;
				it++;
			}
			else
			{
				it = m->enemies.erase(it);
			}
		}
		else
		{
			if(length(it->m_position - m->playerPosition) < it->m_radius + m->playerRadius)
			{
				if(it->m_mayNotPass)
				{
					it = m->enemies.erase(it);
				}
				else
				{
					m->state = Lost;
					++it;
				}
			}
			else
			{
				++it;
			}
		}
	}

	if(m->pendingEnemies.empty() && m->enemies.empty())
	{
		if(length(m->playerPosition) < 0.05f + m->playerRadius)
		{
			m->state = Won;
		}
	}
}

void World::render(Batcher& batcher) const
{
	for(vec2 n : m->playerPositionsList)
	{
		batcher.addCircle(n, 0.03f, { 0.663f, 0.663f, 0.663f, 1.f });
	}

	if(m->pendingEnemies.empty() && m->enemies.empty())
	{
		batcher.addCircle(vec2::zero, 0.05f, { 0.f, 0.f, 0.f, 1.f });
	}
	else
	{
		batcher.addCircle(vec2::zero, 0.05f, { 0.5f, 0.5f, 0.5f, 0.5f });
	}

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

World::State World::getState() const
{
	return m->state;
}
