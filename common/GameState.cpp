#include "GameState.h"
#include "DDSLoader.h"
#include "File.h"
#include "Math.h"
#include "OpenGL.h"
#include "StateMachine.h"

#include <cassert>
#include <cstring>
#include <vector>

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

class Batcher
{
public:
	struct Vertex
	{
		vec2 position;
		vec2 uv;
		vec4 colorMul;
		vec4 colorAdd;
	};

	Batcher()
	{
		glGenVertexArrays(1, &m_vertexArray);
		glBindVertexArray(m_vertexArray);

		glGenBuffers(1, &m_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, position));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, uv));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, colorMul));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, colorAdd));
	}

	void addVertex(const vec2& position, const vec2& uv)
	{
		m_vertices.push_back( { position, uv, { 1.f, 1.f, 1.f, 1.f } , { 0.f, 0.f, 0.f } } );
	}

	void flush()
	{
		glBindVertexArray(m_vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), GL_STREAM_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());

		m_vertices.clear();
	}

private:
	std::vector<Vertex> m_vertices;
	GLuint m_vertexArray;
	GLuint m_vertexBuffer;
};

struct GameState::PrivateData
{
	GLuint shaderProgram;
	GLuint texture;
	Batcher batcher;
};

GameState::GameState() : m(new PrivateData)
{
	File vsFile("assets/shaders/game.vs");
	GLuint vs = createShader(GL_VERTEX_SHADER, vsFile.getData(), vsFile.getSize());

	File fsFile("assets/shaders/game.fs");
	GLuint fs = createShader(GL_FRAGMENT_SHADER, fsFile.getData(), fsFile.getSize());

	m->shaderProgram = createProgram(vs, fs);
	glUseProgram(m->shaderProgram);
	glUniform1i(glGetUniformLocation(m->shaderProgram, "u_sampler"), 0);

	File textureFile("assets/images/lena.dds");
	size_t width, height, depth;
	GLenum bindTarget;
	m->texture = loadDDS(textureFile.getData(), textureFile.getSize(), true, width, height, depth, bindTarget);
}

GameState::~GameState()
{
	delete m;
}

void GameState::enter(StateMachine* stateMachine)
{
}

void GameState::leave(StateMachine* stateMachine)
{
}

void GameState::update(StateMachine* stateMachine)
{
}

void GameState::render(StateMachine* stateMachine)
{
	glViewport(0, 0, stateMachine->getFramebufferWidth(), stateMachine->getFramebufferHeight());
	glClearColor(0.75f, 0.375f, 0.375f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m->shaderProgram);

	glBindTexture(GL_TEXTURE_2D, m->texture);

	float sx, sy;
	if(stateMachine->getFramebufferWidth() > stateMachine->getFramebufferHeight())
	{
		sx = float(stateMachine->getFramebufferHeight()) / float(stateMachine->getFramebufferWidth());
		sy = 1.f;
	}
	else
	{
		sx = 1.f;
		sy = float(stateMachine->getFramebufferWidth()) / float(stateMachine->getFramebufferHeight());
	}

	float mvp[] = {
		sx, 0.f, 0.f, 0.f,
		0.f, sy, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	glUniformMatrix4fv(glGetUniformLocation(m->shaderProgram, "u_modelViewProjection"), 1, GL_FALSE, mvp);

	const float w = 0.9f;
	const float h = 0.9f;

	m->batcher.addVertex( { -w, -h }, { 0.f, 0.f } );
	m->batcher.addVertex( { w, -h }, { 1.f, 0.f } );
	m->batcher.addVertex( { w, h }, { 1.f, 1.f } );
	m->batcher.addVertex( { w, h }, { 1.f, 1.f } );
	m->batcher.addVertex( { -w, h }, { 0.f, 1.f } );
	m->batcher.addVertex( { -w, -h }, { 0.f, 0.f } );

	m->batcher.flush();
}

void GameState::mouseDown(StateMachine* stateMachine, float x, float y)
{
	stateMachine->requestState("test");
}
