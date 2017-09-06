#include "MenuState.h"
#include "File.h"
#include "Profile.h"
#include "StateMachine.h"
#include "TextRenderer.h"
#include "Util.h"

#include "Render/Device.h"

struct Uniforms
{
	float mvp[16];
};

struct MenuState::PrivateData
{
	const Render::BlendState* blendState;
	const Render::DepthStencilState* depthStencilState;
	const Render::RasterizerState* rasterizerState;

	Render::ShaderProgram* shaderProgram;
	Render::UniformBuffer* uniformBuffer;
	TextRenderer* textRenderer;
};

MenuState::MenuState(Render::Device* device) : m(new PrivateData)
{
	Render::BlendDesc blendDesc;
	blendDesc.setBlendEnabled(true);
	blendDesc.setColorSourceFactor(Render::BlendSourceFactor::SourceAlpha);
	blendDesc.setColorDestinationFactor(Render::BlendDestinationFactor::OneMinusSourceAlpha);
	m->blendState = device->createBlendState(blendDesc);

	Render::DepthStencilDesc depthStencilDesc;
	depthStencilDesc.setDepthTestEnabled(false);
	depthStencilDesc.setDepthWriteEnabled(false);
	m->depthStencilState = device->createDepthStencilState(depthStencilDesc);

	Render::RasterizerDesc rasterizerDesc;
	rasterizerDesc.setCullMode(Render::CullMode::Back);
	m->rasterizerState = device->createRasterizerState(rasterizerDesc);

	File vsFile("assets/shaders/game.vs");
	Render::VertexShader* vs = device->createVertexShader(vsFile.getData(), vsFile.getSize());

	File fsFile("assets/shaders/game.fs");
	Render::FragmentShader* fs = device->createFragmentShader(fsFile.getData(), fsFile.getSize());

	m->shaderProgram = device->createShaderProgram(vs, fs);
	m->shaderProgram->setUniformBufferBinding("Uniforms", 0);
	m->shaderProgram->setSamplerBinding("u_sampler", 0);

	delete vs;
	delete fs;

	m->uniformBuffer = device->createUniformBuffer(sizeof(Uniforms), Render::BufferUsage::Dynamic, nullptr);

	m->textRenderer = new TextRenderer(device, "opensans_semibold_100px_3px_outline");
}

MenuState::~MenuState()
{
	delete m->textRenderer;
	delete m->shaderProgram;

	delete m;
}

void MenuState::enter(StateMachine* stateMachine)
{
}

void MenuState::leave(StateMachine* stateMachine)
{
}

void MenuState::update(StateMachine* stateMachine)
{
}

void MenuState::render(StateMachine* stateMachine, Render::Device* device, const Render::RenderTarget* renderTarget)
{
	const float SCREEN_WIDTH = 1920.f;
	const float SCREEN_HEIGHT = 1920.f * (float)renderTarget->getHeight() / (float)renderTarget->getWidth();

	device->setViewport(0, 0, renderTarget->getWidth(), renderTarget->getHeight());
	device->clearRenderTargetColor(0.5f, 0.5f, 0.5f, 1.f);

	const float s = float(renderTarget->getWidth()) / SCREEN_WIDTH;
	Uniforms* uniforms = (Uniforms*)device->mapUniformBuffer(m->uniformBuffer, Render::BufferAccess::WriteInvalidateBuffer);
	uniforms->mvp[0] = 2.f * s / float(renderTarget->getWidth());
	uniforms->mvp[1] = 0.f;
	uniforms->mvp[2] = 0.f;
	uniforms->mvp[3] = 0.f,
	uniforms->mvp[4] = 0.f;
	uniforms->mvp[5] = -2.f * s / float(renderTarget->getHeight());
	uniforms->mvp[6] = 0.f;
	uniforms->mvp[7] = 0.f;
	uniforms->mvp[8] = 0.f;
	uniforms->mvp[9] = 0.f;
	uniforms->mvp[10] = 1.f;
	uniforms->mvp[11] = 0.f;
	uniforms->mvp[12] = -1.f;
	uniforms->mvp[13] = 1.f;
	uniforms->mvp[14] = 0.f;
	uniforms->mvp[15] = 1.f;
	device->unmapUniformBuffer(m->uniformBuffer);

	device->bindUniformBuffer(0, m->uniformBuffer);

	device->bindBlendState(m->blendState);
	device->bindDepthStencilState(m->depthStencilState);
	device->bindRasterizerState(m->rasterizerState);

	device->bindShaderProgram(m->shaderProgram);

	float minX, maxX, minY, maxY;

	m->textRenderer->getTextSize("l2p", 0.f, 0.f, minX, maxX, minY, maxY);
	m->textRenderer->addText("l2p", (SCREEN_WIDTH - (maxX - minX)) * 0.5f, SCREEN_HEIGHT * 0.5f - 2.f * m->textRenderer->getLineHeight());

	m->textRenderer->getTextSize("haippirinki", 0.f, 0.f, minX, maxX, minY, maxY);
	m->textRenderer->addText("haippirinki", (SCREEN_WIDTH - (maxX - minX)) * 0.5f, SCREEN_HEIGHT * 0.5f - 1.f * m->textRenderer->getLineHeight());

	std::string level = format("current level: %d %s", Profile::getCurrentLevel(), Profile::getLevelName(Profile::getCurrentLevel()).c_str());
	std::string desc = format("\"%s\"", Profile::getLevelDescription(Profile::getCurrentLevel()).c_str());

	m->textRenderer->getTextSize(level.c_str(), 0.f, 0.f, minX, maxX, minY, maxY);
	m->textRenderer->addText(level.c_str(), (SCREEN_WIDTH - (maxX - minX)) * 0.5f, SCREEN_HEIGHT * 0.5f + 1.f * m->textRenderer->getLineHeight());
	m->textRenderer->getTextSize(desc.c_str(), 0.f, 0.f, minX, maxX, minY, maxY);
	m->textRenderer->addText(desc.c_str(), (SCREEN_WIDTH - (maxX - minX)) * 0.5f, SCREEN_HEIGHT * 0.5f + 2.f * m->textRenderer->getLineHeight());

	m->textRenderer->flush(device);
}

void MenuState::mouseDown(StateMachine* stateMachine, float x, float y)
{
	stateMachine->requestState("game");
}

void MenuState::mouseUp(StateMachine* stateMachine, float x, float y)
{
}

void MenuState::mouseMove(StateMachine* stateMachine, float x, float y)
{
}
