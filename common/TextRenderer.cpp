#include "TextRenderer.h"
#include "Batcher.h"
#include "BitmapFont.h"
#include "DDSLoader.h"
#include "File.h"
#include "OpenGL.h"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <codecvt>
#include <cstring>
#include <locale>
#include <string>

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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, previousTexture);
}

TextRenderer::~TextRenderer()
{
	delete m;
}

void TextRenderer::addText(const char* text, float x, float y)
{
#ifdef _MSC_VER
	auto utf32 = std::wstring_convert<std::codecvt_utf8<unsigned int>, unsigned int>{}.from_bytes(text);
#else
	auto utf32 = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(text);
#endif
	char32_t previous = 0;
	vec2 cursor = { x, y };
	for(char32_t codepoint : utf32)
	{
		cursor.x += m->font.getKerning(previous, codepoint);

		if(codepoint == '\n')
		{
			cursor.x = x;
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

float TextRenderer::getLineHeight() const
{
	return (float)m->font.getLineHeight();
}

void TextRenderer::getTextSize(const char* text, float x, float y, float& minX, float& maxX, float& minY, float& maxY) const
{
	minX = FLT_MAX;
	maxX = -FLT_MAX;
	minY = FLT_MAX;
	maxY = -FLT_MAX;

#ifdef _MSC_VER
	auto utf32 = std::wstring_convert<std::codecvt_utf8<unsigned int>, unsigned int>{}.from_bytes(text);
#else
	auto utf32 = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(text);
#endif
	char32_t previous = 0;
	vec2 cursor = { x, y };
	for(char32_t codepoint : utf32)
	{
		cursor.x += m->font.getKerning(previous, codepoint);

		if(codepoint == '\n')
		{
			cursor.x = x;
			cursor.y += m->font.getLineHeight();
		}
		else
		{
			const BitmapFont::Character* c = m->font.getCharacter(codepoint);

			vec2 xy0 = { float(cursor.x + c->xoffset), float(cursor.y + c->yoffset) };
			vec2 xy1 = { float(cursor.x + c->xoffset + c->width), float(cursor.y + c->yoffset + c->height) };

			minX = std::min(xy0.x, minX);
			maxX = std::max(xy1.x, maxX);
			minY = std::min(xy0.y, minY);
			maxY = std::max(xy1.y, maxY);

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
