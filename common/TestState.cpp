#include "TestState.h"
#include "File.h"
#include "OpenGL.h"
#include "Platform.h"
#include "StateMachine.h"
#include "TextRenderer.h"

#include <memory>

struct TestState::PrivateData
{
	GLuint shaderProgram;

	std::unique_ptr<TextRenderer> textRenderer;
};

TestState::TestState() : m(new PrivateData)
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

TestState::~TestState()
{
	delete m;
}

void TestState::enter(StateMachine* stateMachine)
{
	Platform::putIntPreference("test_count", Platform::getIntPreference("test_count") + 1);
}

void TestState::leave(StateMachine* stateMachine)
{
}

void TestState::update(StateMachine* stateMachine)
{
}

void TestState::render(StateMachine* stateMachine)
{
	glViewport(0, 0, stateMachine->getFramebufferWidth(), stateMachine->getFramebufferHeight());
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	sprintf_s(buf, "\n\ntest count: %d", Platform::getIntPreference("test_count"));
#else
	snprintf(buf, sizeof(buf), "\n\ntest count: %d", Platform::getIntPreference("test_count"));
#endif
	text += buf;
	text += u8"\n\nI ♡ Unicode";

	m->textRenderer->addText(text.c_str());

	m->textRenderer->flush();
}

void TestState::mouseDown(StateMachine* stateMachine, float x, float y)
{
	stateMachine->requestState("menu");
}

void TestState::mouseUp(StateMachine* stateMachine, float x, float y)
{
}

void TestState::mouseMove(StateMachine* stateMachine, float x, float y)
{
}
