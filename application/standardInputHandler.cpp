#include "standardInputHandler.h"
#include "../util/Log.h"

StandardInputHandler::StandardInputHandler(GLFWwindow* window, Camera* camera) : InputHandler(window) {
	this->camera = camera;
}

void StandardInputHandler::keyRepeat(int key, int modifiers)  {
	if (key == GLFW_KEY_Z) {
		camera->move(1, 0, 0);
	}
	if (key == GLFW_KEY_S) {
		camera->move(-1, 0, 0);
	}
	if (key == GLFW_KEY_D) {
		camera->move(0, 1, 0);
	}
	if (key == GLFW_KEY_Q) {
		camera->move(0, -1, 0);
	}
	if (key == GLFW_KEY_SPACE) {
		camera->move(0, 0, 1);
	}
	if (key == GLFW_KEY_LEFT_SHIFT) {
		camera->move(0, 0, -1);
	}
}

void StandardInputHandler::windowResize(int width, int height) {
	glViewport(0, 0, width, height);
}

// void StandardInputHandler::keyDown(int key, int modifiers) {};
// void StandardInputHandler::keyUp(int key, int modifiers) {};
// void StandardInputHandler::keyRepeat(int key, int modifiers) {};
// void StandardInputHandler::mouseDown(int button, int mods) {};
// void StandardInputHandler::mouseUp(int button, int mods) {};
// void StandardInputHandler::mouseMove(double x, double y) {};
void StandardInputHandler::mouseEnter() {
	Log::debug("in");
};
void StandardInputHandler::mouseExit() {
	Log::debug("out");
};
// void StandardInputHandler::scroll(double xOffset, double yOffset) {};