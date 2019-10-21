#pragma once

#include "../engine/input/inputHandler.h"

#include "../engine/event/event.h"
#include "../engine/event/keyEvent.h"
#include "../engine/event/mouseEvent.h"
#include "../engine/event/windowEvent.h"

class Screen;

class StandardInputHandler : public InputHandler {
public: 
	Screen& screen;

	StandardInputHandler(GLFWwindow* window, Screen& screen);

	void onEvent(Event& event) override;

	bool onFrameBufferResize(FrameBufferResizeEvent& event);
	bool onKeyPress(KeyPressEvent& event);
	bool onKeyPressOrRepeat(KeyPressEvent& event);
	bool onDoubleKeyPress(DoubleKeyPressEvent& event);
};