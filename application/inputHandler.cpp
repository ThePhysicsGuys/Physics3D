#include "inputHandler.h"

void InputHandler::keyCallback(int key, int action, int mods) {
	if (action == GLFW_PRESS)
		keyDown(key, mods);
	if (action == GLFW_RELEASE)
		keyUp(key, mods);
	if (action == GLFW_REPEAT)
		keyRepeat(key, mods);
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

InputHandler::InputHandler(GLFWwindow* window) {
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
}