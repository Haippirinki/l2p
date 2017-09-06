#pragma once

#include "Math.h"

namespace Render
{
	class Device;
}

class Batcher
{
public:
	Batcher(Render::Device* device);
	~Batcher();

	void addVertex(const vec2& position, const vec2& uv, const vec4& colorMul = { 1.f, 1.f, 1.f, 1.f }, const vec3& colorAdd = { 0.f, 0.f, 0.f });
	void addQuad(const vec2& xy0, const vec2& xy1, const vec2& uv0, const vec2& uv1, const vec4& colorMul = { 1.f, 1.f, 1.f, 1.f }, const vec3& colorAdd = { 0.f, 0.f, 0.f });
	void addCircle(const vec2& center, float radius, const vec4& colorMul = { 1.f, 1.f, 1.f, 1.f }, const vec3& colorAdd = { 0.f, 0.f, 0.f });

	void flush(Render::Device* device);

private:
	struct PrivateData;
	PrivateData* m;
};
