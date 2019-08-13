#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "../engine/math/vec.h"

#include "keyboard.h"

#include <string>


class InputHandler {
protected:
	GLFWwindow* window;
private:
	bool keys[GLFW_KEY_LAST + 1];
	double timestamp[GLFW_KEY_LAST + 1];
	void keyCallback(int key, int action, int mods);
	void cursorCallback(double x, double y);
	void cursorEnterCallback(int entered);
	void scrollCallback(double xOffset, double yOffset);
	void mouseButtonCallback(int button, int action, int mods);
	void windowSizeCallback(int width, int height);
	void framebufferSizeCallback(int width, int height);
public:
	InputHandler(GLFWwindow* window);
	double keyInterval = 0.2;
	char anyKey = 0;
	bool getKey(Keyboard::Key key);
	Vec2 getMousePos();

	virtual void keyDown(int key, int modifiers) {};
	virtual void keyUp(int key, int modifiers) {};
	virtual void keyRepeat(int key, int modifiers) {};
	virtual void keyDownOrRepeat(int key, int modifiers) {};
	virtual void doubleKeyDown(int key, int modifiers) {};
	virtual void mouseDown(int button, int mods) {};
	virtual void mouseUp(int button, int mods) {};
	virtual void mouseMove(double x, double y) {};
	virtual void mouseEnter() {};
	virtual void mouseExit() {};
	virtual void scroll(double xOffset, double yOffset) {};
	virtual void windowResize(Vec2i dimension) {};
	virtual void framebufferResize(Vec2i dimension) {};
};

