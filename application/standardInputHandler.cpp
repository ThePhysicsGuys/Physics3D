#include "standardInputHandler.h"
#include "../engine/math/mathUtil.h"

StandardInputHandler::StandardInputHandler(GLFWwindow* window, Screen* screen, Camera* camera) : InputHandler(window) {
	this->screen = screen;
	this->camera = camera;
}

void StandardInputHandler::windowResize(int width, int height) {
	glViewport(0, 0, width, height);
	Log::debug("%s", str(screen->screenSize).c_str());
	screen->screenSize = Vec2(width, height);
}

// void StandardInputHandler::keyDown(int key, int modifiers) {};
// void StandardInputHandler::keyUp(int key, int modifiers) {};
// void StandardInputHandler::keyRepeat(int key, int modifiers) {};
// void StandardInputHandler::mouseEnter() {};

void StandardInputHandler::mouseDown(int button, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) rightDragging = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT) leftDragging = true;
};

void StandardInputHandler::mouseUp(int button, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) rightDragging = false;
	if (button == GLFW_MOUSE_BUTTON_LEFT) leftDragging = false;
};

void StandardInputHandler::mouseMove(double x, double y) {
	if (rightDragging) {
		camera->rotate((y - curPos.y) * 0.5, (x - curPos.x) * 0.5, 0);
	}

	if (leftDragging) {
		camera->move((x - curPos.x) * -0.5, (y - curPos.y) * 0.5, 0);
	}

	curPos = Vec2(x, y);
};

void StandardInputHandler::scroll(double xOffset, double yOffset) {
	camera->move(0, 0, -5 * yOffset);
};

void StandardInputHandler::mouseExit() {
	rightDragging = false;
	leftDragging = false;
};
