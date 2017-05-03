#include "TestState.h"
#include "Batcher.h"
#include "BitmapFont.h"
#include "DDSLoader.h"
#include "File.h"
#include "OpenGL.h"
#include "StateMachine.h"

static unsigned int utf8ToUtf32(const unsigned char*& it, const unsigned char* end)
{
	size_t len = end - it;
	const unsigned char* buf = it;
	if(len >= 1)
	{
		if(buf[0] < 0x80)
		{
			it += 1;
			return buf[0];
		}
		else if(len >= 1 && buf[0] >= 0xc2)
		{
			if(len >= 2 && buf[0] < 0xe0)
			{
				if((buf[1] & 0xc0) == 0x80)
				{
					it += 2;
					return ((unsigned int)buf[0] << 6) + buf[1] - 0x3080;
				}
			}
			else if(len >= 3 && buf[0] < 0xf0)
			{
				if((buf[1] & 0xc0) == 0x80 && (buf[2] & 0xc0) == 0x80 && !(buf[0] == 0xe0 && buf[1] < 0xa0))
				{
					it += 3;
					return ((unsigned int)buf[0] << 12) + ((unsigned int)buf[1] << 6) + buf[2] - 0xe2080;
				}
			}
			else if(len >= 4 && buf[0] < 0xf5)
			{
				if((buf[1] & 0xc0) == 0x80 && (buf[2] & 0xc0) == 0x80 && (buf[3] & 0xc0) == 0x80 && !(buf[0] == 0xf0 && buf[1] < 0x90) && !(buf[0] == 0xf4 && buf[1] >= 0x90))
				{
					it += 4;
					return ((unsigned int)buf[0] << 18) + ((unsigned int)buf[1] << 12) + ((unsigned int)buf[2] << 6) + buf[3] - 0x3c82080;
				}
			}
		}
	}

	it += 1;
	return buf[0] + 0xdc00;
}

static void addText(Batcher& batcher, const BitmapFont& font, const char* text, size_t bytes, size_t textureWidth, size_t textureHeight)
{
	const unsigned char* p = (const unsigned char*)text;
	const unsigned char* end = p + bytes;
	unsigned int previous = 0;
	vec2 cursor = { 0.f, 0.f };
	while(p != end)
	{
		unsigned int codepoint = utf8ToUtf32(p, end);
		cursor.x += font.getKerning(previous, codepoint);

		if(const BitmapFont::Character* c = font.getCharacter(codepoint))
		{
			vec2 xy0 = { float(cursor.x + c->xoffset), float(cursor.y + c->yoffset) };
			vec2 xy1 = { float(cursor.x + c->xoffset + c->width), float(cursor.y + c->yoffset + c->height) };

			vec2 uv0 = { float(c->x) / float(textureWidth), float(c->y) / float(textureHeight) };
			vec2 uv1 = { float(c->x + c->width) / float(textureWidth), float(c->y + c->height) / float(textureHeight) };

			batcher.addQuad(xy0, xy1, uv0, uv1);

			cursor.x += c->xadvance;
		}
		else if(codepoint == '\n')
		{
			cursor.x = 0.f;
			cursor.y += font.getLineHeight();
		}
		else
		{
			printf("%u not found\n", codepoint);
		}

		previous = codepoint;
	}
}

struct TestState::PrivateData
{
	GLuint shaderProgram;

	Batcher batcher;

	BitmapFont font;
	GLuint fontTexture;
	size_t fontTextureWidth;
	size_t fontTextureHeight;
	size_t fontTextureDepth;
	GLenum fontTextureBindTarget;
};

TestState::TestState() : m(new PrivateData)
{
	File vsFile("assets/shaders/game.vs");
	GLuint vs = createShader(GL_VERTEX_SHADER, vsFile.getData(), vsFile.getSize());

	File fsFile("assets/shaders/game.fs");
	GLuint fs = createShader(GL_FRAGMENT_SHADER, fsFile.getData(), fsFile.getSize());

	m->shaderProgram = createProgram(vs, fs);
	glUseProgram(m->shaderProgram);
	glUniform1i(glGetUniformLocation(m->shaderProgram, "u_sampler"), 0);

	File fontFile("assets/fonts/eurostile_extended_18px.fnt");
	m->font.init(fontFile.getData(), fontFile.getSize());

	File fontTextureFile("assets/fonts/eurostile_extended_18px_0.dds");
	m->fontTexture = loadDDS(fontTextureFile.getData(), fontTextureFile.getSize(), true, m->fontTextureWidth, m->fontTextureHeight, m->fontTextureDepth, m->fontTextureBindTarget);
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
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(m->shaderProgram);

	float mvp[] = {
		2.f / float(stateMachine->getFramebufferWidth()), 0.f, 0.f, 0.f,
		0.f, -2.f / float(stateMachine->getFramebufferHeight()), 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		-1.f, 1.f, 0.f, 1.f
	};
	glUniformMatrix4fv(glGetUniformLocation(m->shaderProgram, "u_modelViewProjection"), 1, GL_FALSE, mvp);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(m->fontTextureBindTarget, m->fontTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	std::string text(u8"l2p\nhaippirinki");
	addText(m->batcher, m->font, text.c_str(), text.size(), m->fontTextureWidth, m->fontTextureHeight);

	m->batcher.flush();
}

void TestState::mouseDown(StateMachine* stateMachine, float x, float y)
{
	stateMachine->requestState("menu");
}

void TestState::mouseUp(StateMachine* stateMachine, float x, float y)
{
}

void TestState::mouseMove(StateMachine* stateMachine, float x, float y)
{
}
