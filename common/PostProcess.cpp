#include "PostProcess.h"
#include "File.h"
#include "Math.h"
#include "Render/Device.h"

struct Vertex
{
	vec2 position;
};

struct Uniforms
{
	vec2 inverseViewportSize;
};

struct PostProcess::PrivateData
{
	const Render::BlendState* blendState;
	const Render::DepthStencilState* depthStencilState;
	const Render::RasterizerState* rasterizerState;

	Render::ShaderProgram* shaderProgram;
	Render::UniformBuffer* uniformBuffer;

	const Render::Sampler* sampler;
	Render::Texture* texture;

	Render::RenderTarget* multisampleRenderTarget;
	Render::Texture* colorTexture;
	Render::Texture* depthTexture;
	Render::RenderTarget* renderTarget;

	Render::VertexBuffer* vertexBuffer;
	Render::VertexArray* vertexArray;
};

PostProcess::PostProcess(Render::Device* device) : m(new PrivateData)
{
	Render::BlendDesc blendDesc;
	blendDesc.setBlendEnabled(false);
	m->blendState = device->createBlendState(blendDesc);

	Render::DepthStencilDesc depthStencilDesc;
	depthStencilDesc.setDepthTestEnabled(false);
	depthStencilDesc.setDepthWriteEnabled(false);
	m->depthStencilState = device->createDepthStencilState(depthStencilDesc);

	Render::RasterizerDesc rasterizerDesc;
	rasterizerDesc.setCullMode(Render::CullMode::Back);
	m->rasterizerState = device->createRasterizerState(rasterizerDesc);

	File vsFile("assets/shaders/postprocess.vs");
	Render::VertexShader* vs = device->createVertexShader(vsFile.getData(), vsFile.getSize());

	File fsFile("assets/shaders/postprocess.fs");
	Render::FragmentShader* fs = device->createFragmentShader(fsFile.getData(), fsFile.getSize());

	m->shaderProgram = device->createShaderProgram(vs, fs);
	m->shaderProgram->setUniformBufferBinding("Uniforms", 0);
	m->shaderProgram->setSamplerBinding("u_sampler", 0);

	delete vs;
	delete fs;

	m->multisampleRenderTarget = nullptr;
	m->colorTexture = nullptr;
	m->depthTexture = nullptr;
	m->renderTarget = nullptr;
	m->uniformBuffer = nullptr;

	Render::VertexFormat vertexFormat;
	vertexFormat.addAttribute(Render::VertexInputType::F32, 2, false, Render::VertexAttributeType::Float);

	Vertex vertices[6] = 
	{
		{ -1.f, -1.f },
		{ 1.f, -1.f },
		{ -1.f, 1.f },
		{ -1.f, 1.f },
		{ 1.f, -1.f },
		{ 1.f, 1.f }
	};
	m->vertexBuffer = device->createVertexBuffer(6 * sizeof(Vertex), Render::BufferUsage::Static, vertexFormat, vertices);

	m->vertexArray = device->createVertexArray(m->vertexBuffer);

	Render::SamplerDesc samplerDesc;
	samplerDesc.setMinFilter(Render::TextureMinFilter::Linear);
	samplerDesc.setMagFilter(Render::TextureMagFilter::Linear);
	m->sampler = device->createSampler(samplerDesc);
}

PostProcess::~PostProcess()
{
	delete m;
}

void PostProcess::begin(Render::Device* device, const Render::RenderTarget* renderTarget)
{
	if(!m->renderTarget || m->renderTarget->getWidth() != renderTarget->getWidth() || m->renderTarget->getHeight() != renderTarget->getHeight())
	{
		delete m->multisampleRenderTarget;
		delete m->renderTarget;
		delete m->colorTexture;
		delete m->depthTexture;
		delete m->uniformBuffer;

		m->multisampleRenderTarget = device->createRenderTarget(renderTarget->getWidth(), renderTarget->getHeight(), Render::TextureFormat::sRGB8A8, Render::TextureFormat::D32f, 4);

		m->colorTexture = device->createTexture(renderTarget->getWidth(), renderTarget->getHeight(), Render::TextureFormat::sRGB8A8, nullptr);
		m->depthTexture = device->createTexture(renderTarget->getWidth(), renderTarget->getHeight(), Render::TextureFormat::D32f, nullptr);
		m->renderTarget = device->createRenderTarget(m->colorTexture, m->depthTexture);

		Uniforms uniforms = { 1.f / m->multisampleRenderTarget->getWidth(), 1.f / m->multisampleRenderTarget->getHeight() };
		m->uniformBuffer = device->createUniformBuffer(sizeof(Uniforms), Render::BufferUsage::Static, &uniforms);
	}

	device->bindRenderTarget(m->multisampleRenderTarget);
	device->setViewport(0, 0, m->multisampleRenderTarget->getWidth(), m->multisampleRenderTarget->getHeight());
}

void PostProcess::end(Render::Device* device, const Render::RenderTarget* renderTarget)
{
	device->resolveRenderTarget(m->multisampleRenderTarget, m->renderTarget);

	device->bindRenderTarget(renderTarget);
	device->setViewport(0, 0, renderTarget->getWidth(), renderTarget->getHeight());

	device->bindBlendState(m->blendState);
	device->bindDepthStencilState(m->depthStencilState);
	device->bindRasterizerState(m->rasterizerState);

	device->bindShaderProgram(m->shaderProgram);
	device->bindUniformBuffer(0, m->uniformBuffer);

	device->bindSampler(0, m->sampler);
	device->bindTexture(0, m->renderTarget->getColorTexture());

	device->bindVertexArray(m->vertexArray);

	device->draw(Render::Topology::Triangles, 6, 0);
}
