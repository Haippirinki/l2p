#include "TextRenderer.h"
#include "Batcher.h"
#include "BitmapFont.h"
#include "DDSLoader.h"
#include "File.h"
#include "OpenGL.h"

#include <cassert>
#include <cstring>
#include <string>

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

struct TextRenderer::PrivateData
{
	Batcher batcher;

	BitmapFont font;

	GLuint texture;
	size_t textureWidth;
	size_t textureHeight;
	size_t textureDepth;
	GLenum textureBindTarget;
};

TextRenderer::TextRenderer(const char* font) : m(new PrivateData)
{
	File fontFile((std::string("assets/fonts/") + std::string(font) + std::string(".fnt")).c_str());
	m->font.init(fontFile.getData(), fontFile.getSize());

	assert(m->font.getNumPages() == 1);
	File fontTextureFile((std::string("assets/fonts/") + std::string(m->font.getPageName(0))).c_str());

	m->texture = loadDDS(fontTextureFile.getData(), fontTextureFile.getSize(), true, m->textureWidth, m->textureHeight, m->textureDepth, m->textureBindTarget);

	glActiveTexture(GL_TEXTURE0);

	GLint previousTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

	glBindTexture(m->textureBindTarget, m->texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, previousTexture);
}

TextRenderer::~TextRenderer()
{
	delete m;
}

void TextRenderer::addText(const char* text)
{
	size_t bytes = strlen(text);
	const unsigned char* p = (const unsigned char*)text;
	const unsigned char* end = p + bytes;
	unsigned int previous = 0;
	vec2 cursor = { 0.f, 0.f };
	while(p != end)
	{
		unsigned int codepoint = utf8ToUtf32(p, end);
		cursor.x += m->font.getKerning(previous, codepoint);

		if(codepoint == '\n')
		{
			cursor.x = 0.f;
			cursor.y += m->font.getLineHeight();
		}
		else
		{
			const BitmapFont::Character* c = m->font.getCharacter(codepoint);

			vec2 xy0 = { float(cursor.x + c->xoffset), float(cursor.y + c->yoffset) };
			vec2 xy1 = { float(cursor.x + c->xoffset + c->width), float(cursor.y + c->yoffset + c->height) };

			vec2 uv0 = { float(c->x) / float(m->textureWidth), float(c->y) / float(m->textureHeight) };
			vec2 uv1 = { float(c->x + c->width) / float(m->textureWidth), float(c->y + c->height) / float(m->textureHeight) };

			m->batcher.addQuad(xy0, xy1, uv0, uv1);

			cursor.x += c->xadvance;
		}

		previous = codepoint;
	}
}

void TextRenderer::flush()
{
	glBindTexture(m->textureBindTarget, m->texture);

	m->batcher.flush();
}
