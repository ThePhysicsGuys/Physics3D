#pragma once

#include "inputHandler.h"
#include "worlds.h"
#include "view\screen.h"
#include "view\camera.h"

class StandardInputHandler : public InputHandler {
public: 
	Screen& screen;
	Vec2 cursorPosition;
	bool rightDragging = false;
	bool middleDragging = false;
	bool leftDragging = false;

	StandardInputHandler(GLFWwindow* window, Screen& screen);

	void framebufferResize(Vec2i dimension) override;

	void keyDown(int key, int modifiers) override;
	void keyDownOrRepeat(int key, int modifiers) override;
	void doubleKeyDown(int key, int modifiers) override;
	void mouseDown(int button, int mods) override;
	void mouseUp(int button, int mods) override;
	void mouseMove(double x, double y) override;
	void scroll(double xOffset, double yOffset) override;
	void mouseExit() override;
};