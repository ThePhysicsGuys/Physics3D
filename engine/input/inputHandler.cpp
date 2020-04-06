#include "core.h"

#include "inputHandler.h"

#include "keyboard.h"
#include "mouse.h"

#include "event/event.h"
#include "event/keyEvent.h"
#include "event/mouseEvent.h"
#include "event/windowEvent.h"

#pragma region callbacks

void InputHandler::keyCallback(int key, int action, int mods) {
	if (action == Keyboard::PRESS) {

		if (keys[key] == false && glfwGetTime() - timestamp[key] < keyInterval) {
			DoubleKeyPressEvent event(key, mods);
			onEvent(event);
		}  else {
			KeyPressEvent event(key, mods);
			onEvent(event);
		}

		keys[key] = true;
		timestamp[key] = glfwGetTime();
		anyKey++;

	} else if (action == Keyboard::RELEASE) {

		keys[key] = false;
		anyKey--;

		KeyReleaseEvent event(key, mods);
		onEvent(event);

	} else if (action == Keyboard::REPEAT) {
		KeyPressEvent event(key, mods, true);
		onEvent(event);
	}
}

void InputHandler::cursorCallback(double x, double y) {
	MouseMoveEvent event(mousePosition.x, mousePosition.y, x, y);
	onEvent(event);

	if (leftDragging || middleDragging || rightDragging) {
		MouseDragEvent event(mousePosition.x, mousePosition.y, x, y, leftDragging, middleDragging, rightDragging);
		onEvent(event);
	}

	mousePosition = Vec2(x, y);
}

void InputHandler::cursorEnterCallback(int entered) {
	if (entered) {
		MouseEnterEvent event(mousePosition.x, mousePosition.y);
		onEvent(event);
	} else {
		leftDragging = false;
		middleDragging = false;
		rightDragging = false;

		MouseExitEvent event(mousePosition.x, mousePosition.y);
		onEvent(event);
	}
}

void InputHandler::scrollCallback(double xOffset, double yOffset) {
	MouseScrollEvent event(mousePosition.x, mousePosition.y, xOffset, yOffset);
	onEvent(event);
}

void InputHandler::mouseButtonCallback(int button, int action, int mods) {
	if (action == Mouse::PRESS) {

		if (Mouse::RIGHT == button) rightDragging = true;
		if (Mouse::MIDDLE == button) middleDragging = true;
		if (Mouse::LEFT == button) leftDragging = true;

		MousePressEvent event(mousePosition.x, mousePosition.y, button, mods);
		onEvent(event);
	} else if (action == Mouse::RELEASE) {

		if (Mouse::RIGHT == button) rightDragging = false;
		if (Mouse::MIDDLE == button) middleDragging = false;
		if (Mouse::LEFT == button) leftDragging = false;

		MouseReleaseEvent event(mousePosition.x, mousePosition.y, button, mods);
		onEvent(event);
	}
}

void InputHandler::windowSizeCallback(int width, int height) {
	WindowResizeEvent event(width, height);
	onEvent(event);
}

void InputHandler::windowDropCallback(const char* path) {
	std::string stdPath(path);
	WindowDropEvent event(stdPath);
	onEvent(event);
}

void InputHandler::framebufferSizeCallback(int width, int height) {
	FrameBufferResizeEvent event(width, height);
	onEvent(event);
}

#pragma endregion

Vec2 InputHandler::getMousePosition() {
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	return Vec2(x, y);
}

bool InputHandler::getKey(const Keyboard::Key& key) {
	if (key.code < Keyboard::KEY_FIRST || key.code > Keyboard::KEY_LAST)
		return false;

	return keys[key.code];
}

InputHandler::InputHandler(GLFWwindow* window) : window(window) {
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

	glfwSetDropCallback(window, [] (GLFWwindow* window, int count, const char** paths) {
		InputHandler* inputHandler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		for (int i = 0; i < count; i++)
			inputHandler->windowDropCallback(paths[i]);
	});
}