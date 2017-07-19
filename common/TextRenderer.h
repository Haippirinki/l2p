#pragma once

class TextRenderer
{
public:
	TextRenderer(const char* font);
	~TextRenderer();

	void addText(const char* text);

	void flush();

private:
	struct PrivateData;
	PrivateData* m;
};
