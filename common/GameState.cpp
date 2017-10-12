#include "GameState.h"
#include "Batcher.h"
#include "DDSLoader.h"
#include "File.h"
#include "Math.h"
#include "PostProcess.h"
#include "Profile.h"
#include "StateMachine.h"
#include "TextRenderer.h"
#include "World.h"

#include "Render/Device.h"

static vec2 windowToView(StateMachine* stateMachine, const vec2& p)
{
	const float ww = stateMachine->getWindowWidth();
	const float wh = stateMachine->getWindowHeight();
	const float ws = (ww > wh) ? wh : ww;
	return { (2.f * p.x - ww) / ws, (wh - 2.f * p.y) / ws };
}

static vec2 viewToWindow(StateMachine* stateMachine, const vec2& p)
{
	const float ww = stateMachine->getWindowWidth();
	const float wh = stateMachine->getWindowHeight();
	const float ws = (ww > wh) ? wh : ww;
	return { 0.5f * (p.x * ws + ww), 0.5f * (wh - p.y * ws) };
}

struct Uniforms
{
	mat4 mvp;
};

struct GameState::PrivateData
{
	const Render::BlendState* blendState;
	const Render::DepthStencilState* depthStencilState;
	const Render::RasterizerState* rasterizerState;

	Render::ShaderProgram* shaderProgram;
	Render::UniformBuffer* uniformBuffer;

	const Render::Sampler* sampler;
	Render::Texture* texture;

	Batcher* batcher;

	float joystickAreaRadius;
	float joystickStickRadius;
	float joystickMaxOffset;

	bool joystickActive;
	vec2 joystickCenter;
	vec2 joystickPosition;

	World* world;

	double exitTime;

	TextRenderer* textRenderer;

	PostProcess* postProcess;
};

GameState::GameState(Render::Device* device) : m(new PrivateData)
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

	Render::SamplerDesc samplerDesc;
	m->sampler = device->createSampler(samplerDesc);

	File textureFile("assets/images/white.dds");
	m->texture = device->createTexture(textureFile.getData(), textureFile.getSize(), true);

	m->batcher = new Batcher(device);

	m->joystickAreaRadius = 0.2f;
	m->joystickStickRadius = 0.1f;
	m->joystickMaxOffset = 0.1f;

	m->world = nullptr;

	m->textRenderer = new TextRenderer(device, "opensans_semibold_100px_3px_outline");

	m->postProcess = new PostProcess(device);
}

GameState::~GameState()
{
	delete m->postProcess;
	delete m->batcher;
	delete m->texture;
	delete m->shaderProgram;

	delete m;
}

void GameState::enter(StateMachine* stateMachine)
{
	m->joystickActive = false;

	File levelFile(Profile::getLevelName(Profile::getCurrentLevel()).c_str());

	m->world = new World;
	m->world->init(levelFile.getData(), levelFile.getSize());

	m->exitTime = 0.0;
}

void GameState::leave(StateMachine* stateMachine)
{
	delete m->world;
	m->world = nullptr;
}

void GameState::update(StateMachine* stateMachine)
{
	if(m->exitTime == 0.0)
	{
		if(m->joystickActive)
		{
			vec2 d = windowToView(stateMachine, m->joystickPosition) - windowToView(stateMachine, m->joystickCenter);
			m->world->setControl(d / m->joystickMaxOffset);
		}
		else
		{
			m->world->setControl(vec2::zero);
		}

		if(m->joystickActive)
		{
			m->world->update(stateMachine->getTime(), (float)stateMachine->getDeltaTime());
		}

		if(m->world->getState() == World::Lost)
		{
			m->exitTime = stateMachine->getTime() + 0.5;
		}
		else if(m->world->getState() == World::Won)
		{
			if(!Profile::getLevelName(Profile::getCurrentLevel() + 1).empty())
			{
				Profile::setCurrentLevel(Profile::getCurrentLevel() + 1);
			}
			else
			{
				Profile::setCurrentLevel(0);
			}
			m->exitTime = stateMachine->getTime() + 0.5;
		}
	}
}

void GameState::render(StateMachine* stateMachine, Render::Device* device, const Render::RenderTarget* renderTarget)
{
	m->postProcess->begin(device, renderTarget);

	switch(m->world->getState())
	{
	case World::Playing:
		device->clearRenderTargetColor(1.f, 0.9406f, 0.7969f, 1.f);
		break;

	case World::Lost:
		device->clearRenderTargetColor(1.f, 0.5f, 0.5f, 1.f);
		break;

	case World::Won:
		device->clearRenderTargetColor(0.5f, 1.f, 0.5f, 1.f);
		break;
	}

	float hw, hh;
	if(renderTarget->getWidth() > renderTarget->getHeight())
	{
		hw = float(renderTarget->getWidth()) / float(renderTarget->getHeight());
		hh = 1.f;
	}
	else
	{
		hw = 1.f;
		hh = float(renderTarget->getHeight()) / float(renderTarget->getWidth());
	}

	Uniforms* uniforms = (Uniforms*)device->mapUniformBuffer(m->uniformBuffer, Render::BufferAccess::WriteInvalidateBuffer);
	uniforms->mvp = orthographicProjectionMatrix(-hw, hw, -hh, hh, -1.f, 1.f);
	device->unmapUniformBuffer(m->uniformBuffer);

	device->bindUniformBuffer(0, m->uniformBuffer);

	device->bindBlendState(m->blendState);
	device->bindDepthStencilState(m->depthStencilState);
	device->bindRasterizerState(m->rasterizerState);

	device->bindShaderProgram(m->shaderProgram);

	device->bindSampler(0, m->sampler);
	device->bindTexture(0, m->texture);

	m->world->render(stateMachine->getTime(), *m->batcher);

	if(m->joystickActive)
	{
		vec2 c = windowToView(stateMachine, m->joystickCenter);
		vec2 p = windowToView(stateMachine, m->joystickPosition);

		m->batcher->addCircle(c, m->joystickAreaRadius, { 0.5f, 0.5f, 0.5f, 0.333f } );
		m->batcher->addCircle(p, m->joystickStickRadius, { 0.5f, 0.5f, 0.5f, 0.333f } );
	}

	m->batcher->flush(device);
	const float SCREEN_WIDTH = 1920.f;
	const float SCREEN_HEIGHT = 1920.f * (float)renderTarget->getHeight() / (float)renderTarget->getWidth();

	uniforms = (Uniforms*)device->mapUniformBuffer(m->uniformBuffer, Render::BufferAccess::WriteInvalidateBuffer);
	uniforms->mvp = orthographicProjectionMatrix(0.f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.f, -1.f, 1.f);
	device->unmapUniformBuffer(m->uniformBuffer);
	float minX, maxX, minY, maxY;

	if(!m->joystickActive && m->world->getState() == World::Playing && !m->world->isPortalOpen())
	{
		m->textRenderer->getTextSize("Touch screen to continue", 0.f, 0.f, minX, maxX, minY, maxY);
		m->textRenderer->addText("Touch screen to continue", (SCREEN_WIDTH - (maxX - minX)) * 0.5f, SCREEN_HEIGHT * 0.5f - 2.f * m->textRenderer->getLineHeight());
	}

	if(m->world->getState() == World::Won)
	{
		m->textRenderer->getTextSize("Great!", 0.f, 0.f, minX, maxX, minY, maxY);
		m->textRenderer->addText("Great!", (SCREEN_WIDTH - (maxX - minX)) * 0.5f, SCREEN_HEIGHT * 0.5f - 2.f * m->textRenderer->getLineHeight());
	}

	if(m->world->getState() == World::Lost)
	{
		m->textRenderer->getTextSize("Ouch! Click to try again.", 0.f, 0.f, minX, maxX, minY, maxY);
		m->textRenderer->addText("Ouch! Click to try again.", (SCREEN_WIDTH - (maxX - minX)) * 0.5f, SCREEN_HEIGHT * 0.5f - 2.f * m->textRenderer->getLineHeight());
	}

	if(m->world->isPortalOpen() && m->world->getState() == World::Playing)
	{
		m->textRenderer->getTextSize("Portal is open!", 0.f, 0.f, minX, maxX, minY, maxY);
		m->textRenderer->addText("Portal is open!", (SCREEN_WIDTH - (maxX - minX)) * 0.5f, SCREEN_HEIGHT * 0.5f - 2.f * m->textRenderer->getLineHeight());
	 }
	m->textRenderer->flush(device);

	m->postProcess->end(device, renderTarget);
}

void GameState::mouseDown(StateMachine* stateMachine, float x, float y)
{
	if(m->exitTime != 0.0 && m->exitTime <= stateMachine->getTime())
	{
		stateMachine->requestState("menu");
	}
	else
	{
		m->joystickActive = true;
		m->joystickCenter = m->joystickPosition = { x, y };
	}
}

void GameState::mouseUp(StateMachine* stateMachine, float x, float y)
{
	m->joystickActive = false;
}

void GameState::mouseMove(StateMachine* stateMachine, float x, float y)
{
	m->joystickPosition = { x, y };

	if(m->joystickActive)
	{
		vec2 c = windowToView(stateMachine, m->joystickCenter);
		vec2 p = windowToView(stateMachine, m->joystickPosition);
		if(length(p - c) > m->joystickMaxOffset)
		{
			m->joystickCenter = viewToWindow(stateMachine, p + normalize(c - p) * m->joystickMaxOffset);
		}
	}
}
