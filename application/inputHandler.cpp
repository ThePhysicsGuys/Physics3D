
#include "inputHandler.h"
#include "eventHandler.h"

#include "../util/log.h"

void InputHandler::keyCallback(int key, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (keys[key] == false && glfwGetTime() - timestamp[key] < keyInterval) {
			doubleKeyDown(key, mods);
		}
		keys[key] = true;
		timestamp[key] = glfwGetTime();
		anyKey++;
		keyDown(key, mods);
		keyDownOrRepeat(key, mods);
	}

	if (action == GLFW_RELEASE) {
		keys[key] = false;
		anyKey--;
		keyUp(key, mods);
	}

	if (action == GLFW_REPEAT) {
		keyRepeat(key, mods);
		keyDownOrRepeat(key, mods);
	}
}

bool InputHandler::getKey(int key) {
	return keys[key];
}

void InputHandler::cursorCallback(double x, double y) {
	mouseMove(x, y);
}

void InputHandler::cursorEnterCallback(int entered) {
	if (entered)
		mouseEnter();
	else
		mouseExit();
}

void InputHandler::scrollCallback(double xOffset, double yOffset) {
	scroll(xOffset, yOffset);
}

void InputHandler::mouseButtonCallback(int button, int action, int mods) {
	if (action == GLFW_PRESS)
		mouseDown(button, mods);
	if (action == GLFW_RELEASE)
		mouseUp(button, mods);
}

void InputHandler::windowSizeCallback(int width, int height) {
	windowResize(Vec2i(width, height));
}

void InputHandler::framebufferSizeCallback(int width, int height) {
	framebufferResize(Vec2i(width, height));
}

Vec2 InputHandler::getMousePos() {
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	return Vec2(x, y);
}

InputHandler::InputHandler(GLFWwindow* window) : window(window){
	glfwSetWindowUserPointer(window, this);

	glfwSetKeyCallback(window, [] (GLFWwindow* window, int key, int scancode, int action, int mods) {
		static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->keyCallback(key, action, mods);
	});

	glfwSetCursorPosCallback(window, [] (GLFWwindow* window, double x, double y) {
		static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->cursorCallback(x, y);
	});

	glfwSetCursorEnterCallback(window, [] (GLFWwindow* window, int entered) {
		static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->cursorEnterCallback(entered);
	});

	glfwSetScrollCallback(window, [] (GLFWwindow* window, double xOffset, double yOffset) {
		static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->scrollCallback(xOffset, yOffset);
	});

	glfwSetMouseButtonCallback(window, [] (GLFWwindow* window, int button, int action, int mods) {
		static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->mouseButtonCallback(button, action, mods);
	});

	glfwSetWindowSizeCallback(window, [] (GLFWwindow* window, int width, int height) {
		static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->windowSizeCallback(width, height);
	});

	glfwSetFramebufferSizeCallback(window, [] (GLFWwindow* window, int width, int height) {
		static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->framebufferSizeCallback(width, height);
	});
}