#include "Application.h"

#include <OpenGL/gl3.h>

Application::Application()
{
}

Application::~Application()
{
}

void Application::update(int width, int height)
{
	glViewport(0, 0, width, height);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Application::mouseDown(float x, float y)
{
}
