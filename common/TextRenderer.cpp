#include "TextRenderer.h"
#include "Batcher.h"
#include "BitmapFont.h"
#include "DDSLoader.h"
#include "File.h"

#include "Render/Device.h"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <codecvt>
#include <cstring>
#include <locale>
#include <string>

struct TextRenderer::PrivateData
{
	Batcher* batcher;

	BitmapFont font;

	const Render::Sampler* sampler;
	Render::Texture* texture;
};

TextRenderer::TextRenderer(Render::Device* device, const char* font) : m(new PrivateData)
{
	File fontFile((std::string("assets/fonts/") + std::string(font) + std::string(".fnt")).c_str());
	m->font.init(fontFile.getData(), fontFile.getSize());

	assert(m->font.getNumPages() == 1);
	File fontTextureFile((std::string("assets/fonts/") + std::string(m->font.getPageName(0))).c_str());

	Render::SamplerDesc samplerDesc;
	samplerDesc.setMinFilter(Render::TextureMinFilter::LinearMipmapLinear);
	samplerDesc.setMagFilter(Render::TextureMagFilter::Linear);
	m->sampler = device->createSampler(samplerDesc);

	m->texture = device->createTexture(fontTextureFile.getData(), fontTextureFile.getSize(), true);

	m->batcher = new Batcher(device);
}

TextRenderer::~TextRenderer()
{
	delete m->texture;
	delete m->batcher;

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

			vec2 uv0 = { float(c->x) / float(m->texture->getWidth()), float(c->y) / float(m->texture->getHeight()) };
			vec2 uv1 = { float(c->x + c->width) / float(m->texture->getWidth()), float(c->y + c->height) / float(m->texture->getHeight()) };

			m->batcher->addQuad(xy0, xy1, uv0, uv1);

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

void TextRenderer::flush(Render::Device* device)
{
	device->bindSampler(0, m->sampler);
	device->bindTexture(0, m->texture);

	m->batcher->flush(device);
}
