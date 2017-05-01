#pragma once

#include "Math.h"

#include <cstddef>

class Batcher;

class World
{
public:
	enum State
	{
		Playing,
		Lost,
		Won
	};

	World();
	~World();

	void init(const void* data, size_t size);

	void update(float dt);
	void render(Batcher& batcher) const;

	void setControl(const vec2& control);

	State getState() const;

private:
	struct PrivateData;
	PrivateData* m;
};
