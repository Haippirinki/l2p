#define _CRT_SECURE_NO_WARNINGS 1
#include "World.h"
#include "Batcher.h"

#include <cassert>
#include <cctype>
#include <cmath>
#include <cstring>
#include <list>
#include <string>

std::list<std::string> getLines(const char* data, size_t size)
{
	std::list<std::string> lines;

	const char* p = nullptr;
	for(size_t i = 0; i < size; i++)
	{
		if(!p)
		{
			if(!isspace(data[i]))
			{
				p = data + i;
			}
		}
		else
		{
			if(data[i] == '\n')
			{
				lines.push_back(std::string(p, i - (p - data)));
				p = nullptr;
			}
		}
	}

	if(p)
	{
		lines.push_back(std::string(p, size - (p - data)));
	}

	return lines;
}

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
}

World::~World()
{
	delete m;
}

void World::init(const void* data, size_t size)
{
	std::list<std::string> lines = getLines((const char*)data, size);
	for(std::list<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it)
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
				m->enemies.push_back({ { cosf(a) * distance, sinf(a) * distance }, speed, 0.05f,{ 0.f, speed - 0.5f, 1.f, 1.f } });
			}
		}
	}
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
