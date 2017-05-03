#pragma once

#include <cstddef>
#include <cstdint>

class BitmapFont
{
public:
	struct Character
	{
		uint32_t id;
		uint16_t x;
		uint16_t y;
		uint16_t width;
		uint16_t height;
		int16_t xoffset;
		int16_t yoffset;
		int16_t xadvance;
		uint8_t page;
		uint8_t chnl;
	};

	BitmapFont();
	~BitmapFont();

	void init(const void* data, size_t size);

	uint8_t getNumPages() const;
	const char* getPageName(uint8_t page) const;

	const Character* getCharacter(uint32_t id) const;

	int16_t getKerning(uint32_t first, uint32_t second) const;

	uint16_t getBase() const;
	uint16_t getLineHeight() const;

private:
	struct PrivateData;
	PrivateData* m;
};
