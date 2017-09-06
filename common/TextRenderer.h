#pragma once

namespace Render
{
	class Device;
}

class TextRenderer
{
public:
	TextRenderer(Render::Device* device, const char* font);
	~TextRenderer();

	void addText(const char* text, float x, float y);

	float getLineHeight() const;
	void getTextSize(const char* text, float x, float y, float& minX, float& maxX, float& minY, float& maxY) const;

	void flush(Render::Device* device);

private:
	struct PrivateData;
	PrivateData* m;
};
