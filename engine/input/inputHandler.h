#pragma once

#include "keyboard.h"
#include <GLFW/glfw3.h>

namespace P3D::Engine {

class Event;

class InputHandler {
private:
	void setCallBacks(GLFWwindow* window);

protected:
	GLFWwindow* window;

private:
	bool keys[GLFW_KEY_LAST + 1];
	double timestamp[GLFW_KEY_LAST + 1];
	void keyCallback(int key, int action, int mods);
	void cursorCallback(double x, double y);
	void cursorEnterCallback(int entered);
	void windowDropCallback(const char* path);
	void scrollCallback(double xOffset, double yOffset);
	void mouseButtonCallback(int button, int action, int mods);
	void windowSizeCallback(int width, int height);
	void framebufferSizeCallback(int width, int height);

	double keyInterval = 0.2;
public:
	char anyKey = 0;

	bool rightDragging = false;
	bool middleDragging = false;
	bool leftDragging = false;

	Vec2 mousePosition;
	Vec4 viewport;

	InputHandler(GLFWwindow* window);

	bool getKey(const Key& key) const;
	void setKey(const Key& key, bool pressed);
	Vec2 getMousePosition() const;

	virtual void onEvent(Event& event) {};
};

};