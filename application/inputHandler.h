#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

class InputHandler {
private:
	void keyCallback(int key, int action, int mods);
	void cursorCallback(double x, double y);
	void cursorEnterCallback(int entered);
	void scrollCallback(double xOffset, double yOffset);
	void mouseButtonCallback(int button, int action, int mods);
public:
	InputHandler(GLFWwindow* window);
	virtual void keyDown(int key, int modifiers) {};
	virtual void keyUp(int key, int modifiers) {};
	virtual void keyRepeat(int key, int modifiers) {};
	virtual void mouseDown(int button, int mods) {};
	virtual void mouseUp(int button, int mods) {};
	virtual void mouseMove(double x, double y) {};
	virtual void mouseEnter() {};
	virtual void mouseExit() {};
	virtual void scroll(double xOffset, double yOffset) {};
};

