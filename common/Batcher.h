#pragma once

#include "Math.h"

class Batcher
{
public:
	Batcher();
	~Batcher();

	void addVertex(const vec2& position, const vec2& uv, const vec4& colorMul = { 1.f, 1.f, 1.f, 1.f }, const vec3& colorAdd = { 0.f, 0.f, 0.f });

	void addCircle(const vec2& center, float radius, const vec4& colorMul = { 1.f, 1.f, 1.f, 1.f }, const vec3& colorAdd = { 0.f, 0.f, 0.f });

	void flush();

private:
	struct PrivateData;
	PrivateData* m;
};
