#include "standardInputHandler.h"

StandardInputHandler::StandardInputHandler(GLFWwindow* window, Camera* camera) : InputHandler(window) {
	this->camera = camera;
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
// void StandardInputHandler::mouseEnter() {};
// void StandardInputHandler::mouseExit() {};
// void StandardInputHandler::scroll(double xOffset, double yOffset) {};