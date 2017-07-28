#pragma once

class TextRenderer
{
public:
	TextRenderer(const char* font);
	~TextRenderer();

	void addText(const char* text, float x, float y);

	float getLineHeight() const;
	void getTextSize(const char* text, float x, float y, float& minX, float& maxX, float& minY, float& maxY) const;

	void flush();

private:
	struct PrivateData;
	PrivateData* m;
};
