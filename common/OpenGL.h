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

#ifdef ANDROID
#include <GLES3/gl3.h>
#endif

#ifdef _WIN32
#include <win32/glcorearb.h>
#include <win32/GL_3_3.h>
#endif

#include <cstddef>

#ifndef GL_EXT_texture_filter_anisotropic
#define GL_EXT_texture_filter_anisotropic 1
#define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

extern const char* glslVersion;

GLuint createShader(GLenum type, const void* data, size_t size);
GLuint createProgram(GLuint vs, GLuint fs);
