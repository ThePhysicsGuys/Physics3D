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
	Vec2 mousePosition;
	bool rightDragging = false;
	bool middleDragging = false;
	bool leftDragging = false;

	StandardInputHandler(GLFWwindow* window, Screen& screen);

	void onEvent(Event& event) override;

	bool onFrameBufferResize(FrameBufferResizeEvent& event);
	bool onKeyPress(KeyPressEvent& event);
	bool onKeyPressOrRepeat(KeyPressEvent& event);
	bool onDoubleKeyPress(DoubleKeyPressEvent& event);
	bool onMousePress(MousePressEvent& event);
	bool onMouseRelease(MouseReleaseEvent& event);
	bool onMouseMove(MouseMoveEvent& event);
	bool onMouseScroll(MouseScrollEvent& event);
	bool onMouseExit(MouseExitEvent& event);
};