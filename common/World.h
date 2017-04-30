#pragma once

#include "Math.h"

#include <cstddef>

class Batcher;

class World
{
public:
	World();
	~World();

	void init(const void* data, size_t size);

	bool update(float dt);
	void render(Batcher& batcher) const;

	void setControl(const vec2& control);

private:
	struct PrivateData;
	PrivateData* m;
};
