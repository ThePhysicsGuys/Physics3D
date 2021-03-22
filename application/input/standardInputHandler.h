#pragma once

#include "../engine/input/inputHandler.h"
#include "../engine/event/keyEvent.h"
#include "../engine/event/windowEvent.h"

namespace P3D::Application {

class Screen;

class StandardInputHandler : public Engine::InputHandler {
public:
	Screen& screen;

	StandardInputHandler(GLFWwindow* window, Screen& screen);

	void onEvent(Engine::Event& event) override;

	bool onFrameBufferResize(Engine::FrameBufferResizeEvent& event);
	bool onWindowResize(Engine::WindowResizeEvent& event);
	bool onKeyPress(Engine::KeyPressEvent& event);
	bool onKeyPressOrRepeat(Engine::KeyPressEvent& event);
	bool onDoubleKeyPress(Engine::DoubleKeyPressEvent& event);
};

};