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

	void update(double t, float dt);
	void render(double t, Batcher& batcher) const;

	void setControl(const vec2& control);

	State getState() const;
	bool isPortalOpen() const;

private:
	struct PrivateData;
	PrivateData* m;
};
