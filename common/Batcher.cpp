#include "Batcher.h"

#include "Render/Device.h"

#include <cmath>
#include <cstring>
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

	Render::VertexBuffer* vertexBuffer;
	Render::VertexArray* vertexArray;
};

Batcher::Batcher(Render::Device* device) : m(new PrivateData)
{
	Render::VertexFormat vertexFormat;
	vertexFormat.addAttribute(Render::VertexInputType::F32, 2, false, Render::VertexAttributeType::Float);
	vertexFormat.addAttribute(Render::VertexInputType::F32, 2, false, Render::VertexAttributeType::Float);
	vertexFormat.addAttribute(Render::VertexInputType::F32, 4, false, Render::VertexAttributeType::Float);
	vertexFormat.addAttribute(Render::VertexInputType::F32, 3, false, Render::VertexAttributeType::Float);

	m->vertexBuffer = device->createVertexBuffer(3 * 65536 * sizeof(Vertex), Render::BufferUsage::Stream, vertexFormat, nullptr);
	m->vertexArray = device->createVertexArray(m->vertexBuffer);

	m->vertices.reserve(3 * 65536);
}

Batcher::~Batcher()
{
	delete m->vertexArray;
	delete m->vertexBuffer;

	delete m;
}

void Batcher::addVertex(const vec2& position, const vec2& uv, const vec4& colorMul, const vec3& colorAdd)
{
	m->vertices.push_back(Vertex{ position, uv, colorMul, colorAdd });
}

void Batcher::addQuad(const vec2& xy0, const vec2& xy1, const vec2& uv0, const vec2& uv1, const vec4& colorMul, const vec3& colorAdd)
{
	addVertex({ xy0.x, xy0.y }, { uv0.x, uv0.y }, colorMul, colorAdd);
	addVertex({ xy0.x, xy1.y }, { uv0.x, uv1.y }, colorMul, colorAdd);
	addVertex({ xy1.x, xy0.y }, { uv1.x, uv0.y }, colorMul, colorAdd);
	addVertex({ xy0.x, xy1.y }, { uv0.x, uv1.y }, colorMul, colorAdd);
	addVertex({ xy1.x, xy1.y }, { uv1.x, uv1.y }, colorMul, colorAdd);
	addVertex({ xy1.x, xy0.y }, { uv1.x, uv0.y }, colorMul, colorAdd);
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

void Batcher::flush(Render::Device* device)
{
	if(!m->vertices.empty())
	{
		Vertex* p = (Vertex*)device->mapVertexBuffer(m->vertexBuffer, Render::BufferAccess::WriteInvalidateBuffer);
		memcpy(p, m->vertices.data(), 3 * 65536 * sizeof(Vertex));
		device->unmapVertexBuffer(m->vertexBuffer);

		device->bindVertexArray(m->vertexArray);

		device->draw(Render::Topology::Triangles, m->vertices.size(), 0);

		m->vertices.clear();
		m->vertices.reserve(3 * 65536);
	}
}
