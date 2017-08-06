#include "MenuState.h"
#include "File.h"
#include "OpenGL.h"
#include "Profile.h"
#include "StateMachine.h"
#include "TextRenderer.h"
#include "Util.h"

#include <memory>

struct MenuState::PrivateData
{
	GLuint shaderProgram;

	std::unique_ptr<TextRenderer> textRenderer;
};

MenuState::MenuState() : m(new PrivateData)
{
	File vsFile("assets/shaders/game.vs");
	GLuint vs = createShader(GL_VERTEX_SHADER, vsFile.getData(), vsFile.getSize());

	File fsFile("assets/shaders/game.fs");
	GLuint fs = createShader(GL_FRAGMENT_SHADER, fsFile.getData(), fsFile.getSize());

	m->shaderProgram = createProgram(vs, fs);
	glUseProgram(m->shaderProgram);
	glUniform1i(glGetUniformLocation(m->shaderProgram, "u_sampler"), 0);

	m->textRenderer = std::unique_ptr<TextRenderer>(new TextRenderer("opensans_semibold_100px_3px_outline"));
}

MenuState::~MenuState()
{
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

void MenuState::render(StateMachine* stateMachine)
{
	const float SCREEN_WIDTH = 1920.f;
	const float SCREEN_HEIGHT = 1920.f * (float)stateMachine->getFramebufferHeight() / (float)stateMachine->getFramebufferWidth();

	glViewport(0, 0, stateMachine->getFramebufferWidth(), stateMachine->getFramebufferHeight());
	glClearColor(0.5f, 0.5f, 0.5f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(m->shaderProgram);

	const float s = float(stateMachine->getFramebufferWidth()) / SCREEN_WIDTH;
	float mvp[] = {
		2.f * s / float(stateMachine->getFramebufferWidth()), 0.f, 0.f, 0.f,
		0.f, -2.f * s / float(stateMachine->getFramebufferHeight()), 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		-1.f, 1.f, 0.f, 1.f
	};
	glUniformMatrix4fv(glGetUniformLocation(m->shaderProgram, "u_modelViewProjection"), 1, GL_FALSE, mvp);

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

	m->textRenderer->flush();
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
