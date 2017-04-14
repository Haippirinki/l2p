#include "Application.h"

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include <OpenGLES/ES3/gl.h>
#else
#include <OpenGL/gl3.h>
#endif
#endif

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
