#include "Device.h"

#include <common/DDSLoader.h>
#include <common/OpenGL.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <vector>

using namespace Render;

static void* mapBuffer(size_t size, GLuint buffer, GLenum target, GLenum binding, GLbitfield access)
{
	GLint previousBuffer;
	glGetIntegerv(binding, &previousBuffer);

	glBindBuffer(target, buffer);
	void* p = glMapBufferRange(target, 0, size, access);

	glBindBuffer(GL_ARRAY_BUFFER, previousBuffer);

	return p;
}

static void unmapBuffer(GLuint buffer, GLenum target, GLenum binding)
{
	GLint previousBuffer;
	glGetIntegerv(binding, &previousBuffer);

	glBindBuffer(target, buffer);

	glUnmapBuffer(target);

	glBindBuffer(target, previousBuffer);
}

static GLenum getFormat(TextureFormat textureFormat)
{
	switch(textureFormat)
	{
	default:
	case TextureFormat::None:
		return 0;

	case TextureFormat::R8:
		return GL_RED;

	case TextureFormat::RG8:
		return GL_RG;

	case TextureFormat::RGB8:
		return GL_RGB;

	case TextureFormat::RGBA8:
	case TextureFormat::sRGB8A8:
		return GL_RGBA;

	case TextureFormat::D16:
	case TextureFormat::D24:
	case TextureFormat::D32f:
		return GL_DEPTH_COMPONENT;
	}
}

static GLenum getType(TextureFormat textureFormat)
{
	switch(textureFormat)
	{
	default:
	case TextureFormat::None:
		return 0;

	case TextureFormat::R8:
	case TextureFormat::RG8:
	case TextureFormat::RGB8:
	case TextureFormat::RGBA8:
	case TextureFormat::sRGB8A8:
		return GL_UNSIGNED_BYTE;

	case TextureFormat::D16:
		return GL_UNSIGNED_SHORT;

	case TextureFormat::D24:
		return GL_UNSIGNED_INT;

	case TextureFormat::D32f:
		return GL_FLOAT;
	}
}

struct Device::PrivateData
{
	std::vector<std::pair<SamplerDesc, Sampler*>> samplers;
	std::vector<std::pair<BlendDesc, BlendState*>> blendStates;
	std::vector<std::pair<DepthStencilDesc, DepthStencilState*>> depthStencilStates;
	std::vector<std::pair<RasterizerDesc, RasterizerState*>> rasterizerStates;

	const Sampler* currentSampler[MaxTextureUnits];
	const BlendState* currentBlendState;
	const DepthStencilState* currentDepthStencilState;
	const RasterizerState* currentRasterizerState;

	float currentBlendColor[4];

	float maxAnisotropy;
};

Device::Device() : m(new PrivateData)
{
	for(size_t i = 0; i < MaxTextureUnits; ++i)
	{
		m->currentSampler[i] = nullptr;
	}

	m->currentBlendState = nullptr;
	m->currentDepthStencilState = nullptr;
	m->currentRasterizerState = nullptr;

	m->currentBlendColor[0] = 0.f;
	m->currentBlendColor[1] = 0.f;
	m->currentBlendColor[2] = 0.f;
	m->currentBlendColor[3] = 0.f;

	GLint maxTextureUnits;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	assert(size_t(maxTextureUnits) >= MaxTextureUnits);

	GLint maxUniformBufferBindings;
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBufferBindings);
	assert(size_t(maxUniformBufferBindings) >= MaxUniformBufferBindings);

	GLint maxVertexAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttributes);
	assert(size_t(maxVertexAttributes) >= MaxVertexAttributes);

	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	m->maxAnisotropy = 1.f;
	for(int i = 0; i < numExtensions; i++)
	{
		if(strcmp((const char*)glGetStringi(GL_EXTENSIONS, i), "GL_EXT_texture_filter_anisotropic") == 0)
		{
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m->maxAnisotropy);
		}
	}
}

Device::~Device()
{
	delete m;
}

RenderTarget* Device::createRenderTarget(Texture* colorTexture, Texture* depthTexture)
{
	assert(colorTexture || depthTexture);

	size_t width = colorTexture ? colorTexture->getWidth() : depthTexture->getWidth();
	size_t height = colorTexture ? colorTexture->getHeight() : depthTexture->getHeight();

	return new RenderTarget(colorTexture, depthTexture, width, height);
}

RenderTarget* Device::createRenderTarget(size_t width, size_t height, TextureFormat colorFormat, TextureFormat depthFormat, size_t samples)
{
	return new RenderTarget(width, height, colorFormat, depthFormat, samples);
}

VertexShader* Device::createVertexShader(const void* data, size_t size)
{
	VertexShader* shader = new VertexShader;
	if(!shader->init(data, size))
	{
		delete shader;
		return nullptr;
	}
	return shader;
}

FragmentShader* Device::createFragmentShader(const void* data, size_t size)
{
	FragmentShader* shader = new FragmentShader;
	if(!shader->init(data, size))
	{
		delete shader;
		return nullptr;
	}
	return shader;
}

ShaderProgram* Device::createShaderProgram(const VertexShader* vertexShader, const FragmentShader* fragmentShader)
{
	ShaderProgram* program = new ShaderProgram;
	if(!program->init(vertexShader->m_shader, fragmentShader->m_shader))
	{
		delete program;
		return nullptr;
	}
	return program;
}

VertexBuffer* Device::createVertexBuffer(size_t size, BufferUsage bufferUsage, const VertexFormat& vertexFormat, const void* data)
{
	return new VertexBuffer(size, bufferUsage, vertexFormat, data);
}

void* Device::mapVertexBuffer(VertexBuffer* vertexBuffer, BufferAccess bufferAccess)
{
	return mapBuffer(vertexBuffer->m_size, vertexBuffer->m_buffer, GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING, GLenum(bufferAccess));
}

void Device::unmapVertexBuffer(VertexBuffer* vertexBuffer)
{
	unmapBuffer(vertexBuffer->m_buffer, GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING);
}

IndexBuffer* Device::createIndexBuffer(size_t size, BufferUsage bufferUsage, IndexFormat indexFormat, const void* data)
{
	return new IndexBuffer(size, bufferUsage, indexFormat, data);
}

void* Device::mapIndexBuffer(IndexBuffer* indexBuffer, BufferAccess bufferAccess)
{
	return mapBuffer(indexBuffer->m_size, indexBuffer->m_buffer, GL_ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER_BINDING, GLenum(bufferAccess));
}

void Device::unmapIndexBuffer(IndexBuffer* indexBuffer)
{
	unmapBuffer(indexBuffer->m_buffer, GL_ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER_BINDING);
}

UniformBuffer* Device::createUniformBuffer(size_t size, BufferUsage bufferUsage, const void* data)
{
	return new UniformBuffer(size, bufferUsage, data);
}

void* Device::mapUniformBuffer(UniformBuffer* uniformBuffer, BufferAccess bufferAccess)
{
	return mapBuffer(uniformBuffer->m_size, uniformBuffer->m_buffer, GL_UNIFORM_BUFFER, GL_UNIFORM_BUFFER_BINDING, GLenum(bufferAccess));
}

void Device::unmapUniformBuffer(UniformBuffer* uniformBuffer)
{
	unmapBuffer(uniformBuffer->m_buffer, GL_UNIFORM_BUFFER, GL_UNIFORM_BUFFER_BINDING);
}

VertexArray* Device::createVertexArray(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer)
{
	return new VertexArray(vertexBuffer, indexBuffer);
}

Texture* Device::createTexture(size_t width, size_t height, TextureFormat textureFormat, const void* data)
{
	return new Texture(width, height, textureFormat, data);
}

Texture* Device::createTexture(const void* data, size_t size, bool sRGB)
{
	size_t width, height, depth;
	GLenum bindTarget;
	GLuint texture = loadDDS(data, size, sRGB, width, height, depth, bindTarget);
	if(texture)
	{
		return new Texture(texture, bindTarget, width, height, depth);
	}
	return nullptr;
}

const Sampler* Device::createSampler(const SamplerDesc& samplerDesc)
{
	for(std::vector<std::pair<SamplerDesc, Sampler*>>::iterator it = m->samplers.begin(); it != m->samplers.end(); ++it)
	{
		if(it->first == samplerDesc)
		{
			return it->second;
		}
	}

	Sampler* sampler = new Sampler(samplerDesc, m->maxAnisotropy);
	m->samplers.push_back(std::pair<SamplerDesc, Sampler*>(samplerDesc, sampler));
	return sampler;
}

const BlendState* Device::createBlendState(const BlendDesc& blendDesc)
{
	for(std::vector<std::pair<BlendDesc, BlendState*>>::iterator it = m->blendStates.begin(); it != m->blendStates.end(); ++it)
	{
		if(it->first == blendDesc)
		{
			return it->second;
		}
	}

	BlendState* blendState = new BlendState(blendDesc);
	m->blendStates.push_back(std::pair<BlendDesc, BlendState*>(blendDesc, blendState));
	return blendState;
}

const DepthStencilState* Device::createDepthStencilState(const DepthStencilDesc& depthStencilDesc)
{
	for(std::vector<std::pair<DepthStencilDesc, DepthStencilState*>>::iterator it = m->depthStencilStates.begin(); it != m->depthStencilStates.end(); ++it)
	{
		if(it->first == depthStencilDesc)
		{
			return it->second;
		}
	}

	DepthStencilState* depthStencilState = new DepthStencilState(depthStencilDesc);
	m->depthStencilStates.push_back(std::pair<DepthStencilDesc, DepthStencilState*>(depthStencilDesc, depthStencilState));
	return depthStencilState;
}

const RasterizerState* Device::createRasterizerState(const RasterizerDesc& rasterizerDesc)
{
	for(std::vector<std::pair<RasterizerDesc, RasterizerState*>>::iterator it = m->rasterizerStates.begin(); it != m->rasterizerStates.end(); ++it)
	{
		if(it->first == rasterizerDesc)
		{
			return it->second;
		}
	}

	RasterizerState* rasterizerState = new RasterizerState(rasterizerDesc);
	m->rasterizerStates.push_back(std::pair<RasterizerDesc, RasterizerState*>(rasterizerDesc, rasterizerState));
	return rasterizerState;
}

void Device::bindRenderTarget(const RenderTarget* renderTarget)
{
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->m_framebuffer);
}

void Device::bindShaderProgram(const ShaderProgram* shaderProgram)
{
	glUseProgram(shaderProgram->m_program);
}

void Device::bindUniformBuffer(unsigned int index, const UniformBuffer* uniformBuffer)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, index, uniformBuffer->m_buffer);
}

void Device::bindTexture(unsigned int unit, const Texture* texture)
{
	assert(unit < MaxTextureUnits);

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(texture->m_bindTarget, texture->m_texture);
}

void Device::bindSampler(unsigned int unit, const Sampler* sampler)
{
	assert(unit < MaxTextureUnits);

	if(sampler == m->currentSampler[unit])
	{
		return;
	}

	glBindSampler(unit, sampler->m_sampler);

	m->currentSampler[unit] = sampler;
}

void Device::bindVertexArray(const VertexArray* vertexArray)
{
	if(vertexArray->m_array)
	{
		glBindVertexArray(vertexArray->m_array);
	}
	else
	{
		glGenVertexArrays(1, &vertexArray->m_array);
		glBindVertexArray(vertexArray->m_array);

		glBindBuffer(GL_ARRAY_BUFFER, vertexArray->m_vertexBuffer->m_buffer);

		size_t offset = 0;

		const VertexFormat& vertexFormat = vertexArray->m_vertexBuffer->m_vertexFormat;

		for(unsigned int i = 0; i < vertexFormat.m_numAttributes; ++i)
		{
			glEnableVertexAttribArray(i);
			switch(vertexFormat.m_attributes[i].attributeType)
			{
			case VertexAttributeType::Float:
				glVertexAttribPointer(i, vertexFormat.m_attributes[i].components, vertexFormat.m_attributes[i].inputType, vertexFormat.m_attributes[i].normalized, GLsizei(vertexFormat.m_stride), (const GLvoid*)offset);
				break;

			case VertexAttributeType::Int:
				glVertexAttribIPointer(i, vertexFormat.m_attributes[i].components, vertexFormat.m_attributes[i].inputType, GLsizei(vertexFormat.m_stride), (const GLvoid*)offset);
				break;
			}

			offset += vertexFormat.m_attributes[i].size;
		}

		if(vertexArray->m_indexBuffer)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArray->m_indexBuffer->m_buffer);
		}
	}
}

void Device::bindBlendState(const BlendState* blendState)
{
	if(blendState == m->currentBlendState)
	{
		return;
	}

	if(blendState->mBlendEnabled)
	{
		glEnable(GL_BLEND);
		glBlendFuncSeparate(blendState->mColorSourceFactor, blendState->mColorDestinationFactor, blendState->mAlphaSourceFactor, blendState->mAlphaDestinationFactor);
		glBlendEquationSeparate(blendState->mColorMode, blendState->mAlphaMode);
	}
	else
	{
		glDisable(GL_BLEND);
	}

	m->currentBlendState = blendState;
}

void Device::bindDepthStencilState(const DepthStencilState* depthStencilState)
{
	if(depthStencilState == m->currentDepthStencilState)
	{
		return;
	}

	if(depthStencilState->mDepthTestEnabled || depthStencilState->mDepthWriteEnabled)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(depthStencilState->mDepthWriteEnabled);
		glDepthFunc(depthStencilState->mDepthComparisonFunction);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	m->currentDepthStencilState = depthStencilState;
}

void Device::bindRasterizerState(const RasterizerState* rasterizerState)
{
	if(rasterizerState == m->currentRasterizerState)
	{
		return;
	}

	if(rasterizerState->mCullFaceEnabled)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(rasterizerState->mCullFace);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}

	m->currentRasterizerState = rasterizerState;
}

void Device::setBlendColor(float r, float g, float b, float a)
{
	if(r == m->currentBlendColor[0] && g == m->currentBlendColor[1] && b == m->currentBlendColor[2] && a == m->currentBlendColor[3])
	{
		return;
	}

	glBlendColor(r, g, b, a);

	m->currentBlendColor[0] = r;
	m->currentBlendColor[1] = g;
	m->currentBlendColor[2] = b;
	m->currentBlendColor[3] = a;
}

void Device::setViewport(int x, int y, size_t width, size_t height)
{
	glViewport(x, y, GLsizei(width), GLsizei(height));
}

void Device::clearRenderTargetColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Device::clearRenderTargetDepth(float depth)
{
#ifdef GL_ES_VERSION_3_0
	glClearDepthf(depth);
#else
	glClearDepth(depth);
#endif
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Device::resolveRenderTarget(const RenderTarget* source, const RenderTarget* target)
{
	blitRenderTarget(source, target, 0, 0, GLint(source->getWidth()), GLint(source->getHeight()), 0, 0, GLint(target->getWidth()), GLint(target->getHeight()));
}

void Device::blitRenderTarget(const RenderTarget* source, const RenderTarget* target, int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1)
{
	GLint previousFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, source->m_framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target->m_framebuffer);

	glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
}

void Device::draw(Topology topology, size_t vertexCount, size_t startVertex)
{
	glDrawArrays(GLenum(topology), GLsizei(startVertex), GLsizei(vertexCount));
}

void Device::drawIndexed(Topology topology, size_t indexCount, size_t startIndex)
{
	glDrawElements(GLenum(topology), GLsizei(indexCount), GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * startIndex));
}

SwapChain* Device::createSwapChain()
{
	return new SwapChain;
}

void Device::updateSwapChain(SwapChain* swapChain, GLuint framebuffer, size_t width, size_t height)
{
	swapChain->m_backBuffer.m_framebuffer = framebuffer;
	swapChain->m_backBuffer.m_width = width;
	swapChain->m_backBuffer.m_height = height;
	swapChain->m_backBuffer.m_samples = 1;
}

RenderTarget::~RenderTarget()
{
	if(m_colorRenderbuffer)
	{
		glDeleteRenderbuffers(1, &m_colorRenderbuffer);
	}

	if(m_depthRenderbuffer)
	{
		glDeleteRenderbuffers(1, &m_depthRenderbuffer);
	}

	if(m_framebuffer)
	{
		glDeleteFramebuffers(1, &m_framebuffer);
	}
}

RenderTarget::RenderTarget()
	: m_framebuffer(0)
	, m_colorTexture(nullptr)
	, m_depthTexture(nullptr)
	, m_colorRenderbuffer(0)
	, m_depthRenderbuffer(0)
	, m_width(0)
	, m_height(0)
	, m_samples(0)
{
}

RenderTarget::RenderTarget(Texture* colorTexture, Texture* depthTexture, size_t width, size_t height)
	: m_framebuffer(0)
	, m_colorTexture(colorTexture)
	, m_depthTexture(depthTexture)
	, m_colorRenderbuffer(0)
	, m_depthRenderbuffer(0)
	, m_width(width)
	, m_height(height)
	, m_samples(1)
{
	assert(m_colorTexture || m_depthTexture);

	GLint previousFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);

	glGenFramebuffers(1, &m_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

	if(colorTexture)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture->m_texture, 0);
	}

	if(depthTexture)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->m_texture, 0);
	}

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
}

RenderTarget::RenderTarget(size_t width, size_t height, TextureFormat colorFormat, TextureFormat depthFormat, size_t samples)
	: m_framebuffer(0)
	, m_colorTexture(nullptr)
	, m_depthTexture(nullptr)
	, m_colorRenderbuffer(0)
	, m_depthRenderbuffer(0)
	, m_width(width)
	, m_height(height)
	, m_samples(samples)
{
	GLint previousFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);

	glGenFramebuffers(1, &m_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

	if(colorFormat != TextureFormat::None)
	{
		glGenRenderbuffers(1, &m_colorRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);

		if(samples > 1)
		{
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, GLsizei(samples), GLenum(colorFormat), GLsizei(width), GLsizei(height));
		}
		else
		{
			glRenderbufferStorage(GL_RENDERBUFFER, GLenum(colorFormat), GLsizei(width), GLsizei(height));
		}

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorRenderbuffer);
	}

	if(depthFormat != TextureFormat::None)
	{
		glGenRenderbuffers(1, &m_depthRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);

		if(samples > 1)
		{
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, GLsizei(samples), GLenum(depthFormat), GLsizei(width), GLsizei(height));
		}
		else
		{
			glRenderbufferStorage(GL_RENDERBUFFER, GLenum(depthFormat), GLsizei(width), GLsizei(height));
		}

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);
	}

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
}

VertexShader::~VertexShader()
{
	if(m_shader)
	{
		glDeleteShader(m_shader);
	}
}

bool VertexShader::init(const void* data, size_t size)
{
	m_shader = createShader(GL_VERTEX_SHADER, data, size);
	return (m_shader != 0);
}

FragmentShader::~FragmentShader()
{
	if(m_shader)
	{
		glDeleteShader(m_shader);
	}
}

bool FragmentShader::init(const void* data, size_t size)
{
	m_shader = createShader(GL_FRAGMENT_SHADER, data, size);
	return (m_shader != 0);
}

ShaderProgram::~ShaderProgram()
{
	if(m_program)
	{
		glDeleteProgram(m_program);
	}
}

void ShaderProgram::setUniformBufferBinding(const char* name, unsigned int binding)
{
	GLuint blockIndex = glGetUniformBlockIndex(m_program, name);
	if(blockIndex != GL_INVALID_INDEX)
	{
		glUniformBlockBinding(m_program, blockIndex, binding);
	}
}

void ShaderProgram::setSamplerBinding(const char* name, unsigned int binding)
{
	GLint location = glGetUniformLocation(m_program, name);
	if(location != -1)
	{
		GLint previousProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);

		glUseProgram(m_program);
		glUniform1i(location, binding);

		glUseProgram(previousProgram);
	}
}

bool ShaderProgram::init(GLuint vertexShader, GLuint fragmentShader)
{
	m_program = createProgram(vertexShader, fragmentShader);
	return (m_program != 0);
}

VertexFormat::VertexFormat()
	: m_stride(0)
	, m_numAttributes(0)
{
}

void VertexFormat::addAttribute(VertexInputType inputType, size_t components, bool normalized, VertexAttributeType attributeType)
{
	assert(m_numAttributes < Device::MaxVertexAttributes);

	size_t size;
	switch(inputType)
	{
	case VertexInputType::I8:
	case VertexInputType::U8:
		size = components;
		break;

	case VertexInputType::I16:
	case VertexInputType::U16:
	case VertexInputType::F16:
		size = 2 * components;
		break;

	case VertexInputType::I32:
	case VertexInputType::U32:
	case VertexInputType::F32:
		size = 4 * components;
		break;
	}

	m_attributes[m_numAttributes++] = { GLenum(inputType), GLint(components), GLboolean(normalized), attributeType, size };
	m_stride += size;
}

VertexBuffer::~VertexBuffer()
{
	if(m_buffer)
	{
		glDeleteBuffers(1, &m_buffer);
	}
}

VertexBuffer::VertexBuffer(size_t size, BufferUsage bufferUsage, const VertexFormat& vertexFormat, const void* data)
{
	GLint previousBuffer;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousBuffer);

	glGenBuffers(1, &m_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBufferData(GL_ARRAY_BUFFER, size, data, GLenum(bufferUsage));

	glBindBuffer(GL_ARRAY_BUFFER, previousBuffer);

	m_size = size;
	m_usage = GLenum(bufferUsage);
	m_vertexFormat = vertexFormat;
}

IndexBuffer::~IndexBuffer()
{
	if(m_buffer)
	{
		glDeleteBuffers(1, &m_buffer);
	}
}

IndexBuffer::IndexBuffer(size_t size, BufferUsage bufferUsage, IndexFormat indexFormat, const void* data)
{
	GLint previousBuffer;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &previousBuffer);

	glGenBuffers(1, &m_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GLenum(bufferUsage));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, previousBuffer);

	m_size = size;
	m_indexFormat = indexFormat;
}

UniformBuffer::~UniformBuffer()
{
	if(m_buffer)
	{
		glDeleteBuffers(1, &m_buffer);
	}
}

UniformBuffer::UniformBuffer(size_t size, BufferUsage bufferUsage, const void* data)
{
	GLint previousBuffer;
	glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &previousBuffer);

	glGenBuffers(1, &m_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
	glBufferData(GL_UNIFORM_BUFFER, size, data, GLenum(bufferUsage));

	glBindBuffer(GL_UNIFORM_BUFFER, previousBuffer);

	m_size = size;
}

VertexArray::~VertexArray()
{
	if(m_array)
	{
		glDeleteVertexArrays(1, &m_array);
	}
}

VertexArray::VertexArray(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer)
	: m_array(0)
	, m_vertexBuffer(vertexBuffer)
	, m_indexBuffer(indexBuffer)
{
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_texture);
}

Texture::Texture(size_t width, size_t height, TextureFormat textureFormat, const void* data)
	: m_bindTarget(GL_TEXTURE_2D)
	, m_width(width)
	, m_height(height)
	, m_depth(1)
{
	GLint previousTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

	glGenTextures(1, &m_texture);
	glBindTexture(m_bindTarget, m_texture);

	glTexImage2D(m_bindTarget, 0, GLenum(textureFormat), GLsizei(width), GLsizei(height), 0, getFormat(textureFormat), getType(textureFormat), data);
	glTexParameteri(m_bindTarget, GL_TEXTURE_MAX_LEVEL, 0);

	glBindTexture(m_bindTarget, previousTexture);
}

Texture::Texture(GLuint texture, GLenum bindTarget, size_t width, size_t height, size_t depth)
	: m_texture(texture)
	, m_bindTarget(bindTarget)
	, m_width(width)
	, m_height(height)
	, m_depth(depth)
{
}

Sampler::Sampler(const SamplerDesc& samplerDesc, float maxAnisotropy)
{
	glGenSamplers(1, &m_sampler);

	glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER, GLint(samplerDesc.m_minFilter));
	glSamplerParameteri(m_sampler, GL_TEXTURE_MAG_FILTER, GLint(samplerDesc.m_magFilter));

	if(maxAnisotropy > 1.f && samplerDesc.m_maxAnisotropy > 1.f)
	{
		glSamplerParameterf(m_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::min(maxAnisotropy, samplerDesc.m_maxAnisotropy));
	}

	glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_S, GLint(samplerDesc.m_addressModeS));
	glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_T, GLint(samplerDesc.m_addressModeT));
	glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_R, GLint(samplerDesc.m_addressModeR));
}

Sampler::~Sampler()
{
	if(m_sampler)
	{
		glDeleteSamplers(1, &m_sampler);
	}
}

BlendState::BlendState(const BlendDesc& blendDesc)
	: mBlendEnabled(blendDesc.mBlendEnabled)
	, mColorSourceFactor(GLenum(blendDesc.mColorSourceFactor))
	, mColorDestinationFactor(GLenum(blendDesc.mColorDestinationFactor))
	, mColorMode(GLenum(blendDesc.mColorMode))
	, mAlphaSourceFactor(GLenum(blendDesc.mAlphaSourceFactor))
	, mAlphaDestinationFactor(GLenum(blendDesc.mAlphaDestinationFactor))
	, mAlphaMode(GLenum(blendDesc.mAlphaMode))
{
}

DepthStencilState::DepthStencilState(const DepthStencilDesc& depthStencilDesc)
	: mDepthTestEnabled(depthStencilDesc.mDepthTestEnabled)
	, mDepthWriteEnabled(depthStencilDesc.mDepthWriteEnabled)
	, mDepthComparisonFunction(depthStencilDesc.mDepthTestEnabled ? GLenum(depthStencilDesc.mDepthComparisonFunction) : GL_ALWAYS)
{
}

RasterizerState::RasterizerState(const RasterizerDesc& rasterizerDesc)
	: mCullFaceEnabled(rasterizerDesc.mCullMode != CullMode::None)
	, mCullFace(GLenum(rasterizerDesc.mCullMode))
{
}

SwapChain::~SwapChain()
{
	m_backBuffer.m_framebuffer = 0;
}

SwapChain::SwapChain()
{
}
