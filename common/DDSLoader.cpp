#include "DDSLoader.h"

#include "OpenGL.h"

#include <cstring>

struct DDS_PIXELFORMAT
{
	unsigned int size;
	unsigned int flags;
	unsigned int fourCC;
	unsigned int RGBBitCount;
	unsigned int RBitMask;
	unsigned int GBitMask;
	unsigned int BBitMask;
	unsigned int ABitMask;
};

struct DDS_HEADER
{
	unsigned int size;
	unsigned int flags;
	unsigned int height;
	unsigned int width;
	unsigned int pitchOrLinearSize;
	unsigned int depth;
	unsigned int mipMapCount;
	unsigned int reserved1[11];
	DDS_PIXELFORMAT ddspf;
	unsigned int caps;
	unsigned int caps2;
	unsigned int caps3;
	unsigned int caps4;
	unsigned int reserved2;
};

struct DDS_HEADER_DXT10
{
	unsigned int dxgiFormat;
	unsigned int resourceDimension;
	unsigned int miscFlag;
	unsigned int arraySize;
	unsigned int miscFlags2;
} ;

#define DDS_PIXELFORMAT_FLAGS_ALPHAPIXELS	0x00000001
#define DDS_PIXELFORMAT_FLAGS_FOURCC		0x00000004
#define DDS_PIXELFORMAT_FLAGS_RGB			0x00000040
#define DDS_PIXELFORMAT_FLAGS_LUMINANCE		0x00020000

#define DDS_HEADER_FLAGS_VOLUME				0x00800000
#define DDS_HEADER_CAPS_MIPMAP				0x00400000
#define DDS_HEADER_CAPS2_CUBEMAP			0x00000200

#define DXGI_FORMAT_R9G9B9E5_SHAREDEXP	67

#define MAKEFOURCC(ch0, ch1, ch2, ch3) (((unsigned int)(unsigned char)(ch0)) | ((unsigned int)(unsigned char)(ch1) << 8) | ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24))

static void getTextureDimensions(const DDS_HEADER* header, size_t& width, size_t& height, size_t& depth, size_t& levels, GLenum& binding, GLenum& bindTarget, GLenum& firstTarget, GLenum& lastTarget)
{
	width = header->width;
	height = header->height;

	levels = (header->caps & DDS_HEADER_CAPS_MIPMAP  && header->mipMapCount > 1) ? header->mipMapCount : 1;

	if(header->flags & DDS_HEADER_FLAGS_VOLUME)
	{
		depth = header->depth;
		binding = GL_TEXTURE_BINDING_3D;
		bindTarget = GL_TEXTURE_3D;
		firstTarget = GL_TEXTURE_3D;
		lastTarget = GL_TEXTURE_3D;
	}
	else if(header->caps2 & DDS_HEADER_CAPS2_CUBEMAP)
	{
		depth = 1;
		binding = GL_TEXTURE_BINDING_CUBE_MAP;
		bindTarget = GL_TEXTURE_CUBE_MAP;
		firstTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		lastTarget = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
	}
	else
	{
		depth = 1;
		binding = GL_TEXTURE_BINDING_2D;
		bindTarget = GL_TEXTURE_2D;
		firstTarget = GL_TEXTURE_2D;
		lastTarget = GL_TEXTURE_2D;
	}
}

static bool getTextureFormat(const DDS_HEADER* header, const DDS_HEADER_DXT10* headerDXT10, bool sRGB, GLint& internalFormat, GLenum& format, GLenum& dataType, bool& compressed, size_t& blockSize)
{
	if(headerDXT10)
	{
		if(headerDXT10->dxgiFormat == DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
		{
			internalFormat = GL_RGB9_E5;
			format = GL_RGB;
			dataType = GL_UNSIGNED_INT_5_9_9_9_REV;
			compressed = false;
			blockSize = 4;
			return true;
		}
	}
	else if(header->ddspf.flags & DDS_PIXELFORMAT_FLAGS_RGB)
	{
		if(header->ddspf.RGBBitCount == 32)
		{
			if(header->ddspf.RBitMask == 0xff && header->ddspf.GBitMask == 0xff00 && header->ddspf.BBitMask == 0xff0000 && header->ddspf.ABitMask == 0xff000000)
			{
				internalFormat = sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
				format = GL_RGBA;
				dataType = GL_UNSIGNED_BYTE;
				compressed = false;
				blockSize = 4;
				return true;
			}

#ifdef GL_BGRA
			if(header->ddspf.RBitMask == 0xff0000 && header->ddspf.GBitMask == 0xff00 && header->ddspf.BBitMask == 0xff && header->ddspf.ABitMask == 0xff000000)
			{
				internalFormat = sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
				format = GL_BGRA;
				dataType = GL_UNSIGNED_BYTE;
				compressed = false;
				blockSize = 4;
				return true;
			}
#endif
		}
	}
	else if(header->ddspf.flags & DDS_PIXELFORMAT_FLAGS_LUMINANCE || header->ddspf.flags & DDS_PIXELFORMAT_FLAGS_ALPHAPIXELS)
	{
		if(header->ddspf.RGBBitCount == 8)
		{
			internalFormat = GL_R8;
			format = GL_RED;
			dataType = GL_UNSIGNED_BYTE;
			compressed = false;
			blockSize = 1;
			return true;
		}
	}
#if defined(GL_EXT_texture_compression_s3tc) && defined(GL_EXT_texture_sRGB)
	else if(header->ddspf.flags & DDS_PIXELFORMAT_FLAGS_FOURCC)
	{
		switch(header->ddspf.fourCC)
		{
		case MAKEFOURCC('D', 'X', 'T', '1'):
			internalFormat = sRGB ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT : GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			compressed = true;
			blockSize = 8;
			return true;

		case MAKEFOURCC('D', 'X', 'T', '2'):
		case MAKEFOURCC('D', 'X', 'T', '3'):
			internalFormat = sRGB ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT : GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			compressed = true;
			blockSize = 16;
			return true;

		case MAKEFOURCC('D', 'X', 'T', '4'):
		case MAKEFOURCC('D', 'X', 'T', '5'):
			internalFormat = sRGB ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT : GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			compressed = true;
			blockSize = 16;
			return true;
		}
	}
#endif
	return false;
}

class DXT1
{
public:
	static const size_t BlockSize = 8;

	static void flipBlock(unsigned char* dst, const unsigned char* src)
	{
		dst[0] = src[0];
		dst[1] = src[1];
		dst[2] = src[2];
		dst[3] = src[3];

		dst[4] = src[7];
		dst[5] = src[6];
		dst[6] = src[5];
		dst[7] = src[4];
	}
};

class DXT3
{
public:
	static const size_t BlockSize = 16;

	static void flipBlock(unsigned char* dst, const unsigned char* src)
	{
		dst[0] = src[6];
		dst[1] = src[7];

		dst[2] = src[4];
		dst[3] = src[5];

		dst[4] = src[2];
		dst[5] = src[3];

		dst[6] = src[0];
		dst[7] = src[1];

		DXT1::flipBlock(dst + 8, src + 8);
	}
};

class DXT5
{
public:
	static const size_t BlockSize = 16;

	static void flipBlock(unsigned char* dst, const unsigned char* src)
	{
		dst[0] = src[0];
		dst[1] = src[1];

		dst[2] = (src[6] >> 4) | (src[7] << 4);
		dst[3] = (src[5] << 4) | (src[7] >> 4);
		dst[4] = (src[5] >> 4) | (src[6] << 4);

		dst[5] = (src[3] >> 4) | (src[4] << 4);
		dst[6] = (src[2] << 4) | (src[4] >> 4);
		dst[7] = (src[2] >> 4) | (src[3] << 4);

		DXT1::flipBlock(dst + 8, src + 8);
	}
};

template<class DXT>
void flipDXT(void* dst, const void* src, size_t width, size_t height)
{
	width = (width + 3) >> 2;
	height = (height + 3) >> 2;

	for(size_t y = 0; y < height; ++y)
	{
		for(size_t x = 0; x < width; ++x)
		{
			const unsigned char* srcBlock = (const unsigned char*)src + (x + y * width) * DXT::BlockSize;
			unsigned char* dstBlock = (unsigned char*)dst + (x + (height - y - 1) * width) * DXT::BlockSize;

			DXT::flipBlock(dstBlock, srcBlock);
		}
	}
}

void flipUncompressed(void* dst, const void* src, size_t width, size_t height, size_t blockSize)
{
	const size_t lineSize = width * blockSize;

	for(size_t y = 0; y < height; ++y)
	{
		const unsigned char* srcBlock = (const unsigned char*)src + y * lineSize;
		unsigned char* dstBlock = (unsigned char*)dst + (height - y - 1) * lineSize;

		memcpy(dstBlock, srcBlock, lineSize);
	}
}

GLuint loadDDS(const void* data, size_t size, bool sRGB, size_t& width, size_t& height, size_t& depth, GLenum& bindTarget)
{
	if(!data || size < 4 + sizeof(DDS_HEADER) || *(const unsigned int*)data != MAKEFOURCC('D', 'D', 'S', ' '))
	{
		return 0;
	}

	const DDS_HEADER* header = (const DDS_HEADER*)(4 + (const char*)data);
	const DDS_HEADER_DXT10* headerDXT10 = nullptr;

	if(header->size != sizeof(DDS_HEADER) || header->ddspf.size != sizeof(DDS_PIXELFORMAT))
	{
		return 0;
	}

	if((header->ddspf.flags & DDS_PIXELFORMAT_FLAGS_FOURCC) && header->ddspf.fourCC == MAKEFOURCC('D', 'X', '1', '0'))
	{
		if(size < 4 + sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10))
		{
			return 0;
		}

		headerDXT10 = (const DDS_HEADER_DXT10*)((const char*)header + sizeof(DDS_HEADER));

		if(headerDXT10->arraySize == 0)
		{
			return 0;
		}

		data = (char*)data + 4 + sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10);
		size -= 4 + sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10);
	}
	else
	{
		data = (char*)data + 4 + sizeof(DDS_HEADER);
		size -= 4 + sizeof(DDS_HEADER);
	}

	size_t levels;
	GLenum binding;
	GLenum firstTarget;
	GLenum lastTarget;
	getTextureDimensions(header, width, height, depth, levels, binding, bindTarget, firstTarget, lastTarget);

	GLint internalFormat;
	GLenum format;
	GLenum dataType;
	bool compressed;
	size_t blockSize;
	if(!getTextureFormat(header, headerDXT10, sRGB, internalFormat, format, dataType, compressed, blockSize))
	{
		return 0;
	}

	GLint previousTexture;
	glGetIntegerv(binding, &previousTexture);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(bindTarget, texture);

	unsigned int* flippedData = nullptr;

	for(GLenum target = firstTarget; target <= lastTarget; ++target)
	{
		size_t levelWidth = width;
		size_t levelHeight = height;
		size_t levelDepth = depth;

		for(size_t level = 0; level < levels; ++level)
		{
#if defined(GL_EXT_texture_compression_s3tc)
			if(compressed)
			{
				size_t levelSize = ((levelWidth + 3) >> 2) * ((levelHeight + 3) >> 2) * levelDepth * blockSize;

				if(bindTarget == GL_TEXTURE_2D)
				{
					if(!flippedData)
					{
						flippedData = new unsigned int[levelSize / 4];
					}

					switch(format)
					{
					case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
						flipDXT<DXT1>(flippedData, data, levelWidth, levelHeight);
						break;

					case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
						flipDXT<DXT3>(flippedData, data, levelWidth, levelHeight);
						break;

					case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
						flipDXT<DXT5>(flippedData, data, levelWidth, levelHeight);
						break;
					}
				}

				if(bindTarget == GL_TEXTURE_3D)
				{
					glCompressedTexImage3D(target, (GLint)level, internalFormat, (GLsizei)levelWidth, (GLsizei)levelHeight, (GLsizei)levelDepth, 0, (GLsizei)levelSize, data);
				}
				else
				{
					glCompressedTexImage2D(target, (GLint)level, internalFormat, (GLsizei)levelWidth, (GLsizei)levelHeight, 0, (GLsizei)levelSize, flippedData ? flippedData : data);
				}

				data = (char*)data + levelSize;
			}
			else
#endif
			{
				size_t levelSize = levelWidth * levelHeight * levelDepth * blockSize;

				if(bindTarget == GL_TEXTURE_2D)
				{
					if(!flippedData)
					{
						flippedData = new unsigned int[levelSize / 4];
					}

					flipUncompressed(flippedData, data, levelWidth, levelHeight, blockSize);
				}

				if(bindTarget == GL_TEXTURE_3D)
				{
					glTexImage3D(target, (GLint)level, internalFormat, (GLsizei)levelWidth, (GLsizei)levelHeight, (GLsizei)levelDepth, 0, format, dataType, data);
				}
				else
				{
					glTexImage2D(target, (GLint)level, internalFormat, (GLsizei)levelWidth, (GLsizei)levelHeight, 0, format, dataType, flippedData ? flippedData : data);
				}

				data = (char*)data + levelSize;
			}

			levelWidth = (levelWidth > 1) ? (levelWidth >> 1) : 1;
			levelHeight = (levelHeight > 1) ? (levelHeight >> 1) : 1;
			levelDepth = (levelDepth > 1) ? (levelDepth >> 1) : 1;
		}
	}

	delete[] flippedData;

	glTexParameteri(bindTarget, GL_TEXTURE_MAX_LEVEL, (GLint)(levels - 1));

	glBindTexture(bindTarget, previousTexture);

	return texture;
}
