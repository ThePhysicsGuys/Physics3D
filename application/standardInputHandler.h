#pragma once

#include "inputHandler.h"
#include "gui\screen.h"
#include "gui\camera.h"

class StandardInputHandler : public InputHandler {
public:
	Screen* screen;
	Camera* camera;
	Vec2 curPos;
	bool rightDragging = false;
	bool leftDragging = false;

	StandardInputHandler(GLFWwindow* window, Screen* screen, Camera* camera);

	void framebufferResize(int width, int height) override;

	void keyDown(int key, int modifiers) override;
	void keyUp(int key, int modifiers) override;
	void keyRepeat(int key, int modifiers) override;
	void keyDownOrRepeat(int key, int modifiers) override;
	
	void mouseDown(int button, int mods) override;
	void mouseUp(int button, int mods) override;
	void mouseMove(double x, double y) override;
	void scroll(double xOffset, double yOffset) override;
	void mouseExit() override;
};