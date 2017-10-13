#include "KTXLoader.h"

#include "OpenGL.h"

#include <cinttypes>

struct KTXHeader
{
	uint8_t identifier[12];
	uint32_t endianness;
	uint32_t glType;
	uint32_t glTypeSize;
	uint32_t glFormat;
	uint32_t glInternalFormat;
	uint32_t glBaseInternalFormat;
	uint32_t pixelWidth;
	uint32_t pixelHeight;
	uint32_t pixelDepth;
	uint32_t numberOfArrayElements;
	uint32_t numberOfFaces;
	uint32_t numberOfMipmapLevels;
	uint32_t bytesOfKeyValueData;
};

const uint8_t ktxIdentifier[12] = { 0xab, 0x4b, 0x54, 0x58, 0x20, 0x31, 0x31, 0xbb, 0x0d, 0x0a, 0x1a, 0x0a };

GLuint loadKTX(const void* data, size_t size, size_t& width, size_t& height, size_t& depth, GLenum& bindTarget)
{
	const KTXHeader* header = (const KTXHeader*)data;

	if(size < sizeof(KTXHeader))
	{
		return 0;
	}

	if(memcmp(data, ktxIdentifier, sizeof(ktxIdentifier)) != 0)
	{
		return 0;
	}

	if(header->endianness != 0x04030201)
	{
		return 0;
	}

	if((header->glType != 0) != (header->glFormat != 0))
	{
		return 0;
	}

	if(size < sizeof(KTXHeader) + header->bytesOfKeyValueData)
	{
		return 0;
	}

	if(header->numberOfArrayElements > 0)
	{
		return 0;
	}

	GLenum binding, firstTarget, lastTarget;
	if(header->pixelWidth == 0)
	{
		return 0;
	}
	else if(header->pixelDepth > 0)
	{
		if(header->pixelHeight == 0 || header->numberOfFaces != 1)
		{
			return 0;
		}

		binding = GL_TEXTURE_BINDING_3D;
		bindTarget = firstTarget = lastTarget = GL_TEXTURE_3D;

	}
	else if(header->pixelHeight > 0)
	{
		if(header->numberOfFaces == 1)
		{
			binding = GL_TEXTURE_BINDING_2D;
			bindTarget = firstTarget = lastTarget = GL_TEXTURE_2D;
		}
		else if(header->numberOfFaces == 6)
		{
			binding = GL_TEXTURE_BINDING_CUBE_MAP;
			bindTarget = GL_TEXTURE_CUBE_MAP;
			firstTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			lastTarget = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if(header->numberOfFaces != 1)
		{
			return 0;
		}

		binding = GL_TEXTURE_BINDING_1D;
		bindTarget = firstTarget = lastTarget = GL_TEXTURE_1D;
	}

	GLuint texture = 0;

	GLint previousUnpackAlignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousUnpackAlignment);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	GLint previousTexture;
	glGetIntegerv(binding, &previousTexture);

	glGenTextures(1, &texture);
	glBindTexture(bindTarget, texture);

	const uint8_t* textureData = ((uint8_t*)data) + sizeof(KTXHeader) + header->bytesOfKeyValueData;

	size_t levelWidth = width = header->pixelWidth;
	size_t levelHeight = height = header->pixelHeight;
	size_t levelDepth = depth = header->pixelDepth;

	for(size_t level = 0; level < (header->numberOfMipmapLevels > 0 ? header->numberOfMipmapLevels : 1); ++level)
	{
		uint32_t faceSize = *(uint32_t*)textureData;
		textureData += sizeof(uint32_t);

		for(GLenum target = firstTarget; target <= lastTarget; ++target)
		{
			if(bindTarget == GL_TEXTURE_3D)
			{
				if(header->glType != 0)
				{
					glTexImage3D(target, (GLint)level, header->glInternalFormat, (GLsizei)levelWidth, (GLsizei)levelHeight, (GLsizei)levelDepth, 0, header->glFormat, header->glType, textureData);
				}
				else
				{
					glCompressedTexImage3D(target, (GLint)level, header->glInternalFormat, (GLsizei)levelWidth, (GLsizei)levelHeight, (GLsizei)levelDepth, 0, (GLsizei)faceSize, textureData);
				}
			}
			else if(bindTarget == GL_TEXTURE_2D)
			{
				if(header->glType != 0)
				{
					glTexImage2D(target, (GLint)level, header->glInternalFormat, (GLsizei)levelWidth, (GLsizei)levelHeight, 0, header->glFormat, header->glType, textureData);
				}
				else
				{
					glCompressedTexImage2D(target, (GLint)level, header->glInternalFormat, (GLsizei)levelWidth, (GLsizei)levelHeight, 0, (GLsizei)faceSize, textureData);
				}
			}
			else
			{
				if(header->glType != 0)
				{
					glTexImage1D(target, (GLint)level, header->glInternalFormat, (GLsizei)levelWidth, 0, header->glFormat, header->glType, textureData);
				}
				else
				{
					glCompressedTexImage1D(target, (GLint)level, header->glInternalFormat, (GLsizei)levelWidth, 0, (GLsizei)faceSize, textureData);
				}
			}

			textureData += (faceSize + 3) & ~3;
		}

		levelWidth = (levelWidth > 1) ? (levelWidth >> 1) : 1;
		levelHeight = (levelHeight > 1) ? (levelHeight >> 1) : 1;
		levelDepth = (levelDepth > 1) ? (levelDepth >> 1) : 1;
	}

	if(header->numberOfMipmapLevels == 0)
	{
		glGenerateMipmap(bindTarget);
	}
	else
	{
		glTexParameteri(bindTarget, GL_TEXTURE_MAX_LEVEL, (GLint)(header->numberOfMipmapLevels - 1));
	}

	glBindTexture(bindTarget, previousTexture);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, previousUnpackAlignment);

	return texture;
}
