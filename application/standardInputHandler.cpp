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

void StandardInputHandler::mouseDown(int button, int mods) {
	if (button == 1) dragging = true;
};
void StandardInputHandler::mouseUp(int button, int mods) {
	if (button == 1) dragging = false;
};
void StandardInputHandler::mouseMove(double x, double y) {
	if (dragging) {
		camera->rotate((y-curPos.y)*0.5, (x-curPos.x)*0.5, 0);
	}
	curPos = Vec2(x, y);
};

void StandardInputHandler::mouseEnter() {

};
void StandardInputHandler::mouseExit() {
	dragging = false;
};
// void StandardInputHandler::scroll(double xOffset, double yOffset) {};