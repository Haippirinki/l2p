#include "TestState.h"
#include "DDSLoader.h"
#include "File.h"
#include "OpenGL.h"
#include "StateMachine.h"

#include <cassert>
#include <cstring>

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
static const char* glslVersion = "#version 300 es\n";
#else
static const char* glslVersion = "#version 330 core\n";
#endif
#endif

static GLuint createShader(GLenum type, const void* data, size_t size)
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

static GLuint createProgram(GLuint vs, GLuint fs)
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

struct TestState::PrivateData
{
	GLuint shaderProgram;

	GLuint fontTexture;
	size_t fontTextureWidth;
	size_t fontTextureHeight;
	size_t fontTextureDepth;
	GLenum fontTextureBindTarget;

	GLuint vertexArray;
	GLuint vertexBuffer[2];
};

TestState::TestState() : m(new PrivateData)
{
	File vsFile("assets/shaders/text.vs");
	GLuint vs = createShader(GL_VERTEX_SHADER, vsFile.getData(), vsFile.getSize());

	File fsFile("assets/shaders/text.fs");
	GLuint fs = createShader(GL_FRAGMENT_SHADER, fsFile.getData(), fsFile.getSize());

	m->shaderProgram = createProgram(vs, fs);
	glUseProgram(m->shaderProgram);
	glUniform1i(glGetUniformLocation(m->shaderProgram, "u_sampler"), 0);

	File fontTextureFile("assets/fonts/eurostile_extended_18px_0.dds");
	m->fontTexture = loadDDS(fontTextureFile.getData(), fontTextureFile.getSize(), true, m->fontTextureWidth, m->fontTextureHeight, m->fontTextureDepth, m->fontTextureBindTarget);

	glGenVertexArrays(1, &m->vertexArray);
	glBindVertexArray(m->vertexArray);

	const float w = 0.5f;
	const float h = 0.5f;
	float vertices[] = { -w, -h, w, -h, w, h, w, h, -w, h, -w, -h };
	float uvs[] = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.f, 0.f };

	glGenBuffers(2, m->vertexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, m->vertexBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m->vertexBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

TestState::~TestState()
{
	delete m;
}

void TestState::enter(StateMachine* stateMachine)
{
}

void TestState::leave(StateMachine* stateMachine)
{
}

void TestState::update(StateMachine* stateMachine)
{
}

void TestState::render(StateMachine* stateMachine)
{
	glViewport(0, 0, stateMachine->getFramebufferWidth(), stateMachine->getFramebufferHeight());
	glClearColor(0.f, 0.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m->shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(m->fontTextureBindTarget, m->fontTexture);

	glBindVertexArray(m->vertexArray);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void TestState::mouseDown(StateMachine* stateMachine, float x, float y)
{
	stateMachine->requestState("menu");
}
