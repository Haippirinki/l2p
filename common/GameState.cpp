#include "GameState.h"
#include "File.h"
#include "OpenGL.h"
#include "StateMachine.h"

#include <cassert>
#include <cstring>

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
const char* glslVersion = "#version 300 es\n";
#else
const char* glslVersion = "#version 330 core\n";
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

struct GameState::PrivateData
{
	GLuint shaderProgram;
	GLuint vertexArray;
	GLuint vertexBuffer;
};

GameState::GameState() : m(new PrivateData)
{
}

GameState::~GameState()
{
	delete m;
}

void GameState::enter(StateMachine* stateMachine)
{
	File vsFile("assets/shaders/basic.vs");
	GLuint vs = createShader(GL_VERTEX_SHADER, vsFile.getData(), vsFile.getSize());

	File fsFile("assets/shaders/basic.fs");
	GLuint fs = createShader(GL_FRAGMENT_SHADER, fsFile.getData(), fsFile.getSize());

	m->shaderProgram = createProgram(vs, fs);

	glGenVertexArrays(1, &m->vertexArray);
	glBindVertexArray(m->vertexArray);

	const float w = 0.5f;
	const float h = 0.5f;
	float vertices[] = { -w, -h, w, -h, w, h, w, h, -w, h, -w, -h };

	glGenBuffers(1, &m->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void GameState::leave(StateMachine* stateMachine)
{
}

void GameState::update(StateMachine* stateMachine)
{
}

void GameState::render(StateMachine* stateMachine)
{
	glViewport(0, 0, stateMachine->getWidth(), stateMachine->getHeight());
	glClearColor(0.f, 0.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m->shaderProgram);
	glBindVertexArray(m->vertexArray);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GameState::mouseDown(StateMachine* stateMachine, float x, float y)
{
	stateMachine->requestState("menu");
}
