#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#else
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#endif
#endif

#ifdef _WIN32
#include <win32/glcorearb.h>
#include <win32/GL_3_3.h>
#endif

extern const char* glslVersion;

GLuint createShader(GLenum type, const void* data, size_t size);
GLuint createProgram(GLuint vs, GLuint fs);
