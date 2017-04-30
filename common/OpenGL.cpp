#include "OpenGL.h"

#include <cassert>
#include <cstdio>
#include <cstring>

#ifdef GL_ES_VERSION_3_0
const char* glslVersion = "#version 300 es\n";
#else
const char* glslVersion = "#version 330 core\n";
#endif

GLuint createShader(GLenum type, const void* data, size_t size)
{
	const GLchar* sources[] = { glslVersion, (const GLchar*)data };
	const GLint sizes[] = { (GLint)strlen(sources[0]), (GLint)size };

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 2, sources, sizes);
	glCompileShader(shader);

	const GLsizei maxInfoLength = 1024;
	GLchar info[maxInfoLength];
	glGetShaderInfoLog(shader, maxInfoLength, 0, info);
	if(info[0])
	{
		printf("shader info log:\n%s", info);
	}

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	assert(status == GL_TRUE);

	return shader;
}

GLuint createProgram(GLuint vs, GLuint fs)
{
	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	const GLsizei maxInfoLength = 1024;
	GLchar info[maxInfoLength];
	glGetProgramInfoLog(program, maxInfoLength, 0, info);
	if(info[0])
	{
		printf("program info log:\n%s", info);
	}

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	assert(status == GL_TRUE);

	return program;
}
