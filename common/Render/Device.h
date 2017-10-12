#pragma once

#include <cstddef>

typedef unsigned char GLboolean;
typedef unsigned int GLenum;
typedef int GLint;
typedef unsigned int GLuint;

namespace Render
{
	class BlendDesc;
	class BlendState;
	class DepthStencilDesc;
	class DepthStencilState;
	class FragmentShader;
	class IndexBuffer;
	class RasterizerDesc;
	class RasterizerState;
	class RenderTarget;
	class Sampler;
	class SamplerDesc;
	class ShaderProgram;
	class SwapChain;
	class Texture;
	class UniformBuffer;
	class VertexArray;
	class VertexBuffer;
	class VertexFormat;
	class VertexShader;

	enum class BufferUsage
	{
		Static = 0x88e4,
		Dynamic = 0x88e8,
		Stream = 0x88e0
	};

	enum class BufferAccess
	{
		WriteInvalidateBuffer = (0x0002 | 0x0008),
		WriteUnsynchronized = (0x0002 | 0x0020)
	};

	enum class IndexFormat
	{
		U8 = 0x1401,
		U16 = 0x1403,
		U32 = 0x1405
	};

	enum class TextureFormat
	{
		None = 0,
		R8 = 0x8229,
		RG8 = 0x822b,
		RGB8 = 0x8051,
		RGBA8 = 0x8058,
		sRGB8A8 = 0x8c43,
		D16 = 0x81a5,
		D24 = 0x81a6,
		D32f = 0x8Cac
	};

	enum class TextureMinFilter
	{
		Nearest = 0x2600,
		Linear = 0x2601,
		NearestMipmapNearest = 0x2700,
		LinearMipmapNearest = 0x2701,
		NearestMipmapLinear = 0x2702,
		LinearMipmapLinear = 0x2703
	};

	enum class TextureMagFilter
	{
		Nearest = 0x2600,
		Linear = 0x2601
	};

	enum class TextureAddressMode
	{
		ClampToEdge = 0x812f,
		MirroredRepeat = 0x8370,
		Repeat = 0x2901
	};

	enum class VertexInputType
	{
		I8 = 0x1400,
		U8 = 0x1401,
		I16 = 0x1402,
		U16 = 0x1403,
		I32 = 0x1404,
		U32 = 0x1405,
		F16 = 0x140B,
		F32 = 0x1406
	};

	enum class VertexAttributeType
	{
		Float,
		Int
	};

	enum class BlendSourceFactor
	{
		Zero = 0x0000,
		One = 0x0001,
		SourceColor = 0x0300,
		OneMinusSourceColor = 0x0301,
		DestinationColor = 0x0306,
		OneMinusDestinationColor = 0x0307,
		SourceAlpha = 0x0302,
		OneMinusSourceAlpha = 0x0303,
		DestinationAlpha = 0x0304,
		OneMinusDestinationAlpha = 0x0305,
		ConstantColor = 0x8001,
		OneMinusConstantColor = 0x8002,
		ConstantAlpha = 0x8003,
		OneMinusConstantAlpha = 0x8004,
		SourceAlphaSaturate = 0x0308
	};

	enum class BlendDestinationFactor
	{
		Zero = 0x0000,
		One = 0x0001,
		SourceColor = 0x0300,
		OneMinusSourceColor = 0x0301,
		DestinationColor = 0x0306,
		OneMinusDestinationColor = 0x0307,
		SourceAlpha = 0x0302,
		OneMinusSourceAlpha = 0x0303,
		DestinationAlpha = 0x0304,
		OneMinusDestinationAlpha = 0x0305,
		ConstantColor = 0x8001,
		OneMinusConstantColor = 0x8002,
		ConstantAlpha = 0x8003,
		OneMinusConstantAlpha = 0x8004
	};

	enum class BlendMode
	{
		Add = 0x8006,
		Subtract = 0x800A,
		ReverseSubtract = 0x800B,
		Min = 0x8007,
		Max = 0x8008
	};

	enum class DepthComparisonFunction
	{
		Never = 0x0200,
		Less = 0x0201,
		Equal = 0x0202,
		LessThanOrEqual = 0x0203,
		Greater = 0x0204,
		NotEqual = 0x0205,
		GreaterThanOrEqual = 0x0206,
		Always = 0x0207
	};

	enum class CullMode
	{
		None = 0,
		Front = 0x0404,
		Back = 0x0405,
		FrontAndBack = 0x0408
	};

	enum class Topology
	{
		Points = 0x0000,
		Lines = 0x0001,
		LineLoop = 0x0002,
		LineStrip = 0x0003,
		Triangles = 0x0004,
		TriangleStrip = 0x0005,
		TriangleFan = 0x0006
	};

	class Device
	{
	public:
		Device();
		~Device();

		void reset();

		RenderTarget* createRenderTarget(Texture* colorTexture, Texture* depthTexture);
		RenderTarget* createRenderTarget(size_t width, size_t height, TextureFormat colorFormat, TextureFormat depthFormat, size_t samples);

		VertexShader* createVertexShader(const void* data, size_t size);
		FragmentShader* createFragmentShader(const void* data, size_t size);
		ShaderProgram* createShaderProgram(const VertexShader* vertexShader, const FragmentShader* fragmentShader);

		VertexBuffer* createVertexBuffer(size_t size, BufferUsage bufferUsage, const VertexFormat& vertexFormat, const void* data);
		void* mapVertexBuffer(VertexBuffer* vertexBuffer, BufferAccess bufferAccess);
		void unmapVertexBuffer(VertexBuffer* vertexBuffer);

		IndexBuffer* createIndexBuffer(size_t size, BufferUsage bufferUsage, IndexFormat indexFormat, const void* data);
		void* mapIndexBuffer(IndexBuffer* indexBuffer, BufferAccess bufferAccess);
		void unmapIndexBuffer(IndexBuffer* indexBuffer);

		UniformBuffer* createUniformBuffer(size_t size, BufferUsage bufferUsage, const void* data);
		void* mapUniformBuffer(UniformBuffer* uniformBuffer, BufferAccess bufferAccess);
		void unmapUniformBuffer(UniformBuffer* uniformBuffer);

		VertexArray* createVertexArray(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer = nullptr);

		Texture* createTexture(size_t width, size_t height, TextureFormat textureFormat, const void* data);
		Texture* createTexture(const void* data, size_t size, bool sRGB);

		const Sampler* createSampler(const SamplerDesc& samplerDesc);

		const BlendState* createBlendState(const BlendDesc& blendDesc);
		const DepthStencilState* createDepthStencilState(const DepthStencilDesc& depthStencilDesc);
		const RasterizerState* createRasterizerState(const RasterizerDesc& rasterizerDesc);

		void bindRenderTarget(const RenderTarget* renderTarget);
		void bindShaderProgram(const ShaderProgram* shaderProgram);
		void bindUniformBuffer(unsigned int index, const UniformBuffer* uniformBuffer);
		void bindTexture(unsigned int index, const Texture* texture);
		void bindSampler(unsigned int index, const Sampler* sampler);
		void bindVertexArray(const VertexArray* vertexArray);
		void bindBlendState(const BlendState* blendState);
		void bindDepthStencilState(const DepthStencilState* depthStencilState);
		void bindRasterizerState(const RasterizerState* rasterizerState);

		void setBlendColor(float r, float g, float b, float a);

		void setViewport(int x, int y, size_t width, size_t height);

		void clearRenderTargetColor(float r, float g, float b, float a);
		void clearRenderTargetDepth(float depth = 1.f);

		void resolveRenderTarget(const RenderTarget* source, const RenderTarget* target);
		void blitRenderTarget(const RenderTarget* source, const RenderTarget* target, int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1);

		void draw(Topology topology, size_t vertexCount, size_t startVertex);
		void drawIndexed(Topology topology, size_t indexCount, size_t startIndex);

		SwapChain* createSwapChain();
		void updateSwapChain(SwapChain* swapChain, GLuint framebuffer, size_t width, size_t height);

		size_t getMaxSamples() const;
		float getMaxAnisotropy() const;

		static const size_t MaxTextureUnits = 16;
		static const size_t MaxUniformBufferBindings = 16;
		static const size_t MaxVertexAttributes = 16;

	private:
		struct PrivateData;
		PrivateData* m;
	};

	class RenderTarget
	{
	public:
		~RenderTarget();

		Texture* getColorTexture() { return m_colorTexture; }
		Texture* getDepthTexture() { return m_depthTexture; }

		size_t getWidth() const { return m_width; }
		size_t getHeight() const { return m_height; }
		size_t getSamples() const { return m_samples; }

	private:
		RenderTarget();
		RenderTarget(Texture* colorTexture, Texture* depthTexture, size_t width, size_t height);
		RenderTarget(size_t width, size_t height, TextureFormat colorFormat, TextureFormat depthFormat, size_t samples);

		GLuint m_framebuffer;

		Texture* m_colorTexture;
		Texture* m_depthTexture;

		GLuint m_colorRenderbuffer;
		GLuint m_depthRenderbuffer;

		size_t m_width;
		size_t m_height;
		size_t m_samples;

		friend class Device;
		friend class SwapChain;
	};

	class VertexShader
	{
	public:
		~VertexShader();

	private:
		bool init(const void* data, size_t size);

		GLuint m_shader;

		friend class Device;
	};

	class FragmentShader
	{
	public:
		~FragmentShader();

	private:
		bool init(const void* data, size_t size);

		GLuint m_shader;

		friend class Device;
	};

	class ShaderProgram
	{
	public:
		~ShaderProgram();

		void setUniformBufferBinding(const char* name, unsigned int binding);
		void setSamplerBinding(const char* name, unsigned int binding);

	private:
		ShaderProgram();
		bool init(GLuint vertexShader, GLuint fragmentShader);

		GLuint m_program;

		bool m_uniformBufferBindingSet[Device::MaxUniformBufferBindings];
		bool m_samplerBindingSet[Device::MaxTextureUnits];

		friend class Device;
	};

	class VertexFormat
	{
	public:
		VertexFormat();

		size_t getStride() const { return m_stride; }

		void addAttribute(VertexInputType inputType, size_t components, bool normalized, VertexAttributeType attributeType);

	private:
		size_t m_stride;
		struct Attribute
		{
			GLenum inputType;
			GLint components;
			GLboolean normalized;
			VertexAttributeType attributeType;
			size_t size;
		};
		size_t m_numAttributes;
		Attribute m_attributes[Device::MaxVertexAttributes];

		friend class Device;
	};

	class VertexBuffer
	{
	public:
		~VertexBuffer();

	private:
		VertexBuffer(size_t size, BufferUsage bufferUsage, const VertexFormat& vertexFormat, const void* data);

		GLuint m_buffer;
		size_t m_size;
		GLenum m_usage;
		VertexFormat m_vertexFormat;

		friend class Device;
	};

	class IndexBuffer
	{
	public:
		~IndexBuffer();

	private:
		IndexBuffer(size_t size, BufferUsage bufferUsage, IndexFormat indexFormat, const void* data);

		GLuint m_buffer;
		size_t m_size;
		IndexFormat m_indexFormat;

		friend class Device;
	};

	class UniformBuffer
	{
	public:
		~UniformBuffer();

	private:
		UniformBuffer(size_t size, BufferUsage bufferUsage, const void* data);

		GLuint m_buffer;
		size_t m_size;

		friend class Device;
	};

	class VertexArray
	{
	public:
		~VertexArray();

	private:
		VertexArray(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer);

		mutable GLuint m_array;

		VertexBuffer* m_vertexBuffer;
		IndexBuffer* m_indexBuffer;

		friend class Device;
	};

	class Texture
	{
	public:
		~Texture();

		size_t getWidth() const { return m_width; }
		size_t getHeight() const { return m_height; }
		size_t getDepth() const { return m_depth; }

	private:
		Texture(size_t width, size_t height, TextureFormat textureFormat, const void* data);
		Texture(GLuint texture, GLenum bindTarget, size_t width, size_t height, size_t depth);

		GLuint m_texture;
		GLenum m_bindTarget;

		size_t m_width;
		size_t m_height;
		size_t m_depth;

		friend class Device;
		friend class RenderTarget;
	};

	class SamplerDesc
	{
	public:
		SamplerDesc()
			: m_minFilter(TextureMinFilter::NearestMipmapLinear)
			, m_magFilter(TextureMagFilter::Linear)
			, m_maxAnisotropy(1.f)
			, m_addressModeS(TextureAddressMode::Repeat)
			, m_addressModeT(TextureAddressMode::Repeat)
			, m_addressModeR(TextureAddressMode::Repeat)
		{}

		void setMinFilter(TextureMinFilter minFilter) { m_minFilter = minFilter; }
		void setMagFilter(TextureMagFilter magFilter) { m_magFilter = magFilter; }

		void setMaxAnisotropy(float maxAnisotropy) { m_maxAnisotropy = maxAnisotropy; }

		void setWrapS(TextureAddressMode addressModeS) { m_addressModeS = addressModeS; }
		void setWrapT(TextureAddressMode addressModeT) { m_addressModeT = addressModeT; }
		void setWrapR(TextureAddressMode addressModeR) { m_addressModeR = addressModeR; }

		bool operator==(const SamplerDesc& rhs) const { return m_minFilter == rhs.m_minFilter && m_magFilter == rhs.m_magFilter && m_maxAnisotropy == rhs.m_maxAnisotropy && m_addressModeS == rhs.m_addressModeS && m_addressModeT == rhs.m_addressModeT && m_addressModeR == rhs.m_addressModeR; }

	private:
		TextureMinFilter m_minFilter;
		TextureMagFilter m_magFilter;

		float m_maxAnisotropy;

		TextureAddressMode m_addressModeS;
		TextureAddressMode m_addressModeT;
		TextureAddressMode m_addressModeR;

		friend class Sampler;
	};

	class Sampler
	{
	private:
		Sampler(const SamplerDesc& samplerDesc, float maxAnisotropy);
		~Sampler();

		GLuint m_sampler;

		friend class Device;
	};

	class BlendDesc
	{
	public:
		BlendDesc()
			: mBlendEnabled(false)
			, mColorSourceFactor(BlendSourceFactor::One)
			, mColorDestinationFactor(BlendDestinationFactor::Zero)
			, mColorMode(BlendMode::Add)
			, mAlphaSourceFactor(BlendSourceFactor::One)
			, mAlphaDestinationFactor(BlendDestinationFactor::Zero)
			, mAlphaMode(BlendMode::Add)
		{}

		void setBlendEnabled(bool blendEnabled) { mBlendEnabled = blendEnabled; }

		void setColorSourceFactor(BlendSourceFactor colorSourceFactor) { mColorSourceFactor = colorSourceFactor; }
		void setColorDestinationFactor(BlendDestinationFactor colorDestinationFactor) { mColorDestinationFactor = colorDestinationFactor; }
		void setColorMode(BlendMode colorMode) { mColorMode = colorMode; }

		void setAlphaSourceFactor(BlendSourceFactor alphaSourceFactor) { mAlphaSourceFactor = alphaSourceFactor; }
		void setAlphaDestinationFactor(BlendDestinationFactor alphaDestinationFactor) { mAlphaDestinationFactor = alphaDestinationFactor; }
		void setAlphaMode(BlendMode alphaMode) { mAlphaMode = alphaMode; }

		bool operator==(const BlendDesc& rhs) const { return mBlendEnabled == rhs.mBlendEnabled && mColorSourceFactor == rhs.mColorSourceFactor && mColorDestinationFactor == rhs.mColorDestinationFactor && mColorMode == rhs.mColorMode && mAlphaSourceFactor == rhs.mAlphaSourceFactor && mAlphaDestinationFactor == rhs.mAlphaDestinationFactor && mAlphaMode == rhs.mAlphaMode; }

	private:
		bool mBlendEnabled;

		BlendSourceFactor mColorSourceFactor;
		BlendDestinationFactor mColorDestinationFactor;
		BlendMode mColorMode;

		BlendSourceFactor mAlphaSourceFactor;
		BlendDestinationFactor mAlphaDestinationFactor;
		BlendMode mAlphaMode;

		friend class BlendState;
	};

	class BlendState
	{
	private:
		BlendState(const BlendDesc& blendDesc);
		~BlendState() {}

		bool mBlendEnabled;

		GLenum mColorSourceFactor;
		GLenum mColorDestinationFactor;
		GLenum mColorMode;

		GLenum mAlphaSourceFactor;
		GLenum mAlphaDestinationFactor;
		GLenum mAlphaMode;

		friend class Device;
	};

	class DepthStencilDesc
	{
	public:
		DepthStencilDesc()
			: mDepthTestEnabled(false)
			, mDepthWriteEnabled(false)
			, mDepthComparisonFunction(DepthComparisonFunction::Less)
		{}

		void setDepthTestEnabled(bool depthTestEnabled) { mDepthTestEnabled = depthTestEnabled; }
		void setDepthWriteEnabled(bool depthWriteEnabled) { mDepthWriteEnabled = depthWriteEnabled; }
		void setDepthComparisonFunction(DepthComparisonFunction depthComparisonFunction) { mDepthComparisonFunction = depthComparisonFunction; }

		bool operator==(const DepthStencilDesc& rhs) const { return mDepthTestEnabled == rhs.mDepthTestEnabled && mDepthWriteEnabled == rhs.mDepthWriteEnabled && mDepthComparisonFunction == rhs.mDepthComparisonFunction; }

	private:
		bool mDepthTestEnabled;
		bool mDepthWriteEnabled;
		DepthComparisonFunction mDepthComparisonFunction;

		friend class DepthStencilState;
	};

	class DepthStencilState
	{
	private:
		DepthStencilState(const DepthStencilDesc& depthStencilDesc);
		~DepthStencilState() {}

		bool mDepthTestEnabled;
		GLboolean mDepthWriteEnabled;
		GLenum mDepthComparisonFunction;

		friend class Device;
	};

	class RasterizerDesc
	{
	public:
		RasterizerDesc()
			: mCullMode(CullMode::None)
		{
		}

		void setCullMode(CullMode cullMode) { mCullMode = cullMode; }

		bool operator==(const RasterizerDesc& rhs) const { return mCullMode == rhs.mCullMode; }

	private:
		CullMode mCullMode;

		friend class RasterizerState;
	};

	class RasterizerState
	{
	private:
		RasterizerState(const RasterizerDesc& rasterizerDesc);
		~RasterizerState() {}

		bool mCullFaceEnabled;
		GLenum mCullFace;

		friend class Device;
	};

	class SwapChain
	{
	public:
		~SwapChain();

		const RenderTarget* getBackBuffer() const { return &m_backBuffer; }

	private:
		SwapChain();

		RenderTarget m_backBuffer;

		friend class Device;
	};
}
