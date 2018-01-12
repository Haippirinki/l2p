#pragma once

#include "Application.h"

class L2PApplication : public Application
{
public:
	L2PApplication();
	virtual ~L2PApplication();

	virtual void init() override;
	virtual void update(float width, float height, double t, double dt) override;
	virtual void render(int width, int height) override;

	virtual void mouseDown(float x, float y) override;
	virtual void mouseUp(float x, float y) override;
	virtual void mouseMove(float x, float y) override;

private:
	struct PrivateData;
	PrivateData* m;
};
