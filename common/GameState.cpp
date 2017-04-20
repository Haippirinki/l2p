#include "GameState.h"
#include "DDSLoader.h"
#include "File.h"
#include "Math.h"
#include "OpenGL.h"
#include "StateMachine.h"

#include <cassert>
#include <cmath>
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

static vec2 windowToView(StateMachine* stateMachine, const vec2& p)
{
	const float ww = stateMachine->getWindowWidth();
	const float wh = stateMachine->getWindowHeight();
	const float ws = (ww > wh) ? wh : ww;
	return { (2.f * p.x - ww) / ws, (wh - 2.f * p.y) / ws };
}

static vec2 viewToWindow(StateMachine* stateMachine, const vec2& p)
{
	const float ww = stateMachine->getWindowWidth();
	const float wh = stateMachine->getWindowHeight();
	const float ws = (ww > wh) ? wh : ww;
	return { 0.5f * (p.x * ws + ww), 0.5f * (wh - p.y * ws) };
}

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
		vec3 colorAdd;
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

	void addVertex(const vec2& position, const vec2& uv, const vec4& colorMul = { 1.f, 1.f, 1.f, 1.f }, const vec3& colorAdd = { 0.f, 0.f, 0.f } )
	{
		m_vertices.push_back( Vertex { position, uv, colorMul, colorAdd } );
	}

	void addCircle(const vec2& center, float radius, const vec4& colorMul = { 1.f, 1.f, 1.f, 1.f }, const vec3& colorAdd = { 0.f, 0.f, 0.f } )
	{
		const int n = 64;
		for(int i = 0; i < n; i++)
		{
			float a0 = 2.f * float(M_PI) * float(i) / float(n);
			float a1 = 2.f * float(M_PI) * float(i + 1) / float(n);

			vec2 d0 { cosf(a0), sinf(a0) };
			vec2 d1 { cosf(a1), sinf(a1) };

			vec2 p0 = center + radius * d0;
			vec2 p1 = center + radius * d1;

			vec2 uvCenter { 0.5f, 0.5f };
			vec2 uv0 = uvCenter + 0.5f * d0;
			vec2 uv1 = uvCenter + 0.5f * d1;

			addVertex(center, uvCenter, colorMul, colorAdd);
			addVertex(p0, uv0, colorMul, colorAdd);
			addVertex(p1, uv1, colorMul, colorAdd);
		}
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

		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_vertices.size());

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
	GLuint whiteTexture;
	Batcher batcher;

	float joystickAreaRadius;
	float joystickStickRadius;
	float joystickMaxOffset;

	bool joystickActive;
	vec2 joystickCenter;
	vec2 joystickPosition;
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

	size_t width, height, depth;
	GLenum bindTarget;
	File textureFile("assets/images/lena.dds");
	m->texture = loadDDS(textureFile.getData(), textureFile.getSize(), true, width, height, depth, bindTarget);

	File whiteTextureFile("assets/images/white.dds");
	m->whiteTexture = loadDDS(whiteTextureFile.getData(), whiteTextureFile.getSize(), true, width, height, depth, bindTarget);

	m->joystickAreaRadius = 0.2f;
	m->joystickStickRadius = 0.1f;
	m->joystickMaxOffset = 0.1f;
}

GameState::~GameState()
{
	delete m;
}

void GameState::enter(StateMachine* stateMachine)
{
	m->joystickActive = false;
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

	if(m->joystickActive)
	{
		vec2 c = windowToView(stateMachine, m->joystickCenter);
		vec2 p = windowToView(stateMachine, m->joystickPosition);

		glBindTexture(GL_TEXTURE_2D, m->whiteTexture);

		m->batcher.addCircle(c, m->joystickAreaRadius, { 0.5f, 0.5f, 0.5f, 1.f } );
		m->batcher.addCircle(p, m->joystickStickRadius, { 0.75f, 0.75f, 0.75f, 1.f } );

		m->batcher.flush();
	}
}

void GameState::mouseDown(StateMachine* stateMachine, float x, float y)
{
	m->joystickActive = true;
	m->joystickCenter = m->joystickPosition = { x, y };
}

void GameState::mouseUp(StateMachine* stateMachine, float x, float y)
{
	m->joystickActive = false;
}

void GameState::mouseMove(StateMachine* stateMachine, float x, float y)
{
	m->joystickPosition = { x, y };

	if(m->joystickActive)
	{
		vec2 c = windowToView(stateMachine, m->joystickCenter);
		vec2 p = windowToView(stateMachine, m->joystickPosition);
		if(length(p - c) > m->joystickMaxOffset)
		{
			m->joystickCenter = viewToWindow(stateMachine, p + normalize(c - p) * m->joystickMaxOffset);
		}
	}
}
