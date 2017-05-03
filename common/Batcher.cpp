#include "Batcher.h"
#include "OpenGL.h"

#include <cmath>
#include <vector>

struct Vertex
{
	vec2 position;
	vec2 uv;
	vec4 colorMul;
	vec3 colorAdd;
};

struct Batcher::PrivateData
{
	std::vector<Vertex> vertices;
	GLuint vertexArray;
	GLuint vertexBuffer;
};


Batcher::Batcher() : m(new PrivateData)
{
	glGenVertexArrays(1, &m->vertexArray);
	glBindVertexArray(m->vertexArray);

	glGenBuffers(1, &m->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m->vertexBuffer);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, position));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, uv));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, colorMul));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, colorAdd));
}

Batcher::~Batcher()
{
	glDeleteBuffers(1, &m->vertexBuffer);
	glDeleteVertexArrays(1, &m->vertexArray);

	delete m;
}

void Batcher::addVertex(const vec2& position, const vec2& uv, const vec4& colorMul, const vec3& colorAdd)
{
	m->vertices.push_back(Vertex{ position, uv, colorMul, colorAdd });
}

void Batcher::addQuad(const vec2& xy0, const vec2& xy1, const vec2& uv0, const vec2& uv1, const vec4& colorMul, const vec3& colorAdd)
{
	addVertex({ xy0.x, xy0.y }, { uv0.x, 1.f - uv0.y }, colorMul, colorAdd);
	addVertex({ xy0.x, xy1.y }, { uv0.x, 1.f - uv1.y }, colorMul, colorAdd);
	addVertex({ xy1.x, xy0.y }, { uv1.x, 1.f - uv0.y }, colorMul, colorAdd);
	addVertex({ xy0.x, xy1.y }, { uv0.x, 1.f - uv1.y }, colorMul, colorAdd);
	addVertex({ xy1.x, xy1.y }, { uv1.x, 1.f - uv1.y }, colorMul, colorAdd);
	addVertex({ xy1.x, xy0.y }, { uv1.x, 1.f - uv0.y }, colorMul, colorAdd);
}

void Batcher::addCircle(const vec2& center, float radius, const vec4& colorMul, const vec3& colorAdd)
{
	const int n = 64;
	for(int i = 0; i < n; i++)
	{
		float a0 = 2.f * 3.14159f * float(i) / float(n);
		float a1 = 2.f * 3.14159f * float(i + 1) / float(n);

		vec2 d0{ cosf(a0), sinf(a0) };
		vec2 d1{ cosf(a1), sinf(a1) };

		vec2 p0 = center + radius * d0;
		vec2 p1 = center + radius * d1;

		vec2 uvCenter{ 0.5f, 0.5f };
		vec2 uv0 = uvCenter + 0.5f * d0;
		vec2 uv1 = uvCenter + 0.5f * d1;

		addVertex(center, uvCenter, colorMul, colorAdd);
		addVertex(p0, uv0, colorMul, colorAdd);
		addVertex(p1, uv1, colorMul, colorAdd);
	}
}

void Batcher::flush()
{
	if(!m->vertices.empty())
	{
		glBindVertexArray(m->vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, m->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m->vertices.size(), m->vertices.data(), GL_STREAM_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m->vertices.size());

		m->vertices.clear();
	}
}
