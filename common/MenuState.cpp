#include "MenuState.h"
#include "File.h"
#include "OpenGL.h"
#include "Profile.h"
#include "StateMachine.h"
#include "TextRenderer.h"

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

	m->textRenderer = std::unique_ptr<TextRenderer>(new TextRenderer("eurostile_extended_18px"));
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
	glViewport(0, 0, stateMachine->getFramebufferWidth(), stateMachine->getFramebufferHeight());
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(m->shaderProgram);

	const float s = 2.f;
	float mvp[] = {
		s / float(stateMachine->getFramebufferWidth()), 0.f, 0.f, 0.f,
		0.f, -s / float(stateMachine->getFramebufferHeight()), 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		-1.f, 1.f, 0.f, 1.f
	};
	glUniformMatrix4fv(glGetUniformLocation(m->shaderProgram, "u_modelViewProjection"), 1, GL_FALSE, mvp);

	std::string text(u8"l2p\nhaippirinki");

	char buf[64];
#ifdef _MSC_VER
	sprintf_s(buf, "\n\ncurrent level: %d %s", Profile::getCurrentLevel(), Profile::getLevelName(Profile::getCurrentLevel()).c_str());
#else
	snprintf(buf, sizeof(buf), "\n\ncurrent level: %d %s", Profile::getCurrentLevel(), Profile::getLevelName(Profile::getCurrentLevel()).c_str());
#endif
	text += buf;
	text += u8"\n\nI ♡ Unicode";

	m->textRenderer->addText(text.c_str());

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
