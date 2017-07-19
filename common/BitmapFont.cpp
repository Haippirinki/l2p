#include "BitmapFont.h"

#include <cassert>
#include <cstring>
#include <string>
#include <vector>

struct BitmapFont::PrivateData
{
	#pragma pack(1)
	struct InfoBlock
	{
		int16_t fontSize;
		uint8_t bitField;
		uint8_t charSet;
		uint16_t stretchH;
		uint8_t aa;
		uint8_t paddingUp;
		uint8_t paddingRight;
		uint8_t paddingDown;
		uint8_t paddingLeft;
		uint8_t spacingHoriz;
		uint8_t spacingVert;
		uint8_t outline;
	};

	#pragma pack(1)
	struct CommonBlock
	{
		uint16_t lineHeight;
		uint16_t base;
		uint16_t scaleW;
		uint16_t scaleH;
		uint16_t pages;
		uint8_t bitField;
		uint8_t alphaChnl;
		uint8_t redChnl;
		uint8_t greenChnl;
		uint8_t blueChnl;
	};

	#pragma pack(1)
	struct KerningPair
	{
		uint32_t first;
		uint32_t second;
		int16_t amount;
	};

	InfoBlock infoBlock;
	std::string fontName;

	CommonBlock commonBlock;

	std::vector<std::string> pageNames;
	std::vector<Character> chars;
	std::vector<KerningPair> kerningPairs;
};

class Deserializer
{
public:
	Deserializer(const void* data, size_t size) : mData(data), mSize(size), mOffset(0)
	{
	}

	void read(void* buffer, size_t bytes)
	{
		assert(mOffset + bytes <= mSize);
		memcpy(buffer, (uint8_t*)mData + mOffset, bytes);
		mOffset += bytes;
	}

	const void* readDirect(size_t bytes)
	{
		assert(mOffset + bytes <= mSize);
		const void* data = (uint8_t*)mData + mOffset;
		mOffset += bytes;
		return data;
	}

	size_t bytesAvailable() const
	{
		return mSize - mOffset;
	}

private:
	const void* mData;
	const size_t mSize;
	ptrdiff_t mOffset;
};

BitmapFont::BitmapFont() : m(new PrivateData)
{
}

BitmapFont::~BitmapFont()
{
	delete m;
}

void BitmapFont::init(const void* data, size_t size)
{
	Deserializer d(data, size);

	char header[4];
	d.read(&header, 4);

	assert(memcmp(header, "BMF", 3) == 0);
	assert(header[3] == 3);

	while(true)
	{
		if(d.bytesAvailable() == 0)
		{
			break;
		}

		uint8_t blockType;
		d.read(&blockType, 1);

		uint32_t blockSize;
		d.read(&blockSize, 4);

		if(blockType == 1)
		{
			d.read(&m->infoBlock, sizeof(m->infoBlock));

			size_t nameSize = blockSize - sizeof(m->infoBlock);
			const char* name = (const char*)d.readDirect(nameSize);
			assert(name);
			m->fontName = std::string(name, nameSize - 1);
		}
		else if(blockType == 2)
		{
			d.read(&m->commonBlock, sizeof(m->commonBlock));
		}
		else if(blockType == 3)
		{
			const char* pagesBlock = (const char*)d.readDirect(blockSize);
			assert(pagesBlock);

			size_t singleNameLength = strnlen(pagesBlock, blockSize) + 1;
			assert(blockSize % singleNameLength == 0);

			for(size_t page = 0; page < blockSize / singleNameLength; ++page)
			{
				m->pageNames.push_back(std::string(pagesBlock + page * singleNameLength, singleNameLength - 1));
			}
		}
		else if(blockType == 4)
		{
			const Character* charsBlock = (const Character*)d.readDirect(blockSize);
			assert(charsBlock);
			assert(blockSize % sizeof(Character) == 0);

			m->chars.insert(m->chars.end(), charsBlock, charsBlock + (blockSize / sizeof(Character)));
		}
		else if(blockType == 5)
		{
			const PrivateData::KerningPair* kerningPairsBlock = (const PrivateData::KerningPair*)d.readDirect(blockSize);
			assert(kerningPairsBlock);
			assert(blockSize % sizeof(PrivateData::KerningPair) == 0);

			m->kerningPairs.insert(m->kerningPairs.end(), kerningPairsBlock, kerningPairsBlock + (blockSize / sizeof(PrivateData::KerningPair)));
		}
		else
		{
			assert(false);
		}
	}
}

uint8_t BitmapFont::getNumPages() const
{
	return uint8_t(m->pageNames.size());
}

const char* BitmapFont::getPageName(uint8_t page) const
{
	return m->pageNames[page].c_str();
}

const BitmapFont::Character* BitmapFont::getCharacter(uint32_t id) const
{
	for(auto it = m->chars.begin(); it != m->chars.end(); ++it)
	{
		if(it->id == id)
		{
			return &(*it);
		}
	}
	for(auto it = m->chars.begin(); it != m->chars.end(); ++it)
	{
		if(it->id == 0xffffffff)
		{
			return &(*it);
		}
	}
	return nullptr;
}

int16_t BitmapFont::getKerning(uint32_t first, uint32_t second) const
{
	for(auto it = m->kerningPairs.begin(); it != m->kerningPairs.end(); ++it)
	{
		if(it->first == first && it->second == second)
		{
			return it->amount;
		}
	}
	return 0;
}

uint16_t BitmapFont::getBase() const
{
	return m->commonBlock.base;
}

uint16_t BitmapFont::getLineHeight() const
{
	return m->commonBlock.lineHeight;
}
