#pragma once

#include "inputHandler.h"

#include "../event/event.h"
#include "../event/keyEvent.h"
#include "../event/mouseEvent.h"
#include "../event/windowEvent.h"

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