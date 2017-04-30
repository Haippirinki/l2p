#pragma once

#include "Math.h"

class Batcher;

class World
{
public:
	World();
	~World();

	bool update(float dt);
	void render(Batcher& batcher) const;

	void setControl(const vec2& control);

private:
	struct PrivateData;
	PrivateData* m;
};
