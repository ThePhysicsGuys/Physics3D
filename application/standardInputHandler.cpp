#include "standardInputHandler.h"
#include "../engine/math/mathUtil.h"
#include "application.h"
#include "gui/picker.h"
#include "objectLibrary.h"
#include <algorithm>
#include <random>

StandardInputHandler::StandardInputHandler(GLFWwindow* window, Screen& screen) : InputHandler(window), screen(screen) {}

void StandardInputHandler::framebufferResize(int width, int height) {
	glViewport(0, 0, width, height);
	screen.screenSize = Vec2(width, height);
	(*screen.eventHandler.windowResizeHandler) (screen, width, height);
}

void StandardInputHandler::keyDownOrRepeat(int key, int modifiers) {
	switch (key) {
	case GLFW_KEY_PAGE_UP:
		setSpeed(getSpeed() * 1.5);
		Log::info("TPS is now: %f", getSpeed());
		break;
	case GLFW_KEY_PAGE_DOWN:
		setSpeed(getSpeed() / 1.5);
		Log::info("TPS is now: %f", getSpeed());
		break;
	case GLFW_KEY_T:
		runTick();
		break;
	case GLFW_KEY_O:
		createDominoAt(Vec3(0.0 + (rand() % 100) * 0.001, 0.5 + (rand() % 100) * 0.001, 0.0 + (rand() % 100) * 0.001), fromEulerAngles(0.2, 0.3, 0.7));
		Log::info("Created domino! There are %d objects in the world! ", screen.world->physicals.size());
		break;
	}
}

void StandardInputHandler::keyDown(int key, int modifiers) {
	switch (key) {
		case GLFW_KEY_P:
			togglePause();
			break;
		case GLFW_KEY_DELETE:
			if (screen.selectedPhysical != nullptr) {
				for (int i = 0; i < screen.world->physicals.size(); i++) {
					if (&screen.world->physicals[i] == screen.selectedPhysical) {
						screen.world->physicals.erase(screen.world->physicals.begin() + i);
						screen.world->selectedPhysical = nullptr;
						screen.selectedPhysical = nullptr;
						break;
					}
				}
			}
			break;
		case GLFW_KEY_F8:
			renderPies = !renderPies;
			break;
	}

	if(key >= GLFW_KEY_F1 && key <= GLFW_KEY_F7) {
		screen.toggleDebugVecType(static_cast<Debug::VecType>(key - GLFW_KEY_F1));
	}
};

void StandardInputHandler::doubleKeyDown(int key, int modifiers) {
	if (key == GLFW_KEY_SPACE)
		screen.camera.toggleFlying();
}

void StandardInputHandler::keyUp(int key, int modifiers) {};
void StandardInputHandler::keyRepeat(int key, int modifiers) {};
// void StandardInputHandler::mouseEnter() {};

void StandardInputHandler::mouseDown(int button, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) rightDragging = true;
	if (button == GLFW_MOUSE_BUTTON_MIDDLE) middleDragging = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT) leftDragging = true;

	(*screen.eventHandler.physicalClickHandler) (screen, screen.intersectedPhysical, screen.intersectedPoint);
	if (screen.intersectedPhysical != nullptr) {
		screen.world->localSelectedPoint = screen.selectedPhysical->part.cframe.globalToLocal(screen.intersectedPoint);
	}
};

void StandardInputHandler::mouseUp(int button, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) rightDragging = false;
	if (button == GLFW_MOUSE_BUTTON_MIDDLE) middleDragging = false;
	if (button == GLFW_MOUSE_BUTTON_LEFT) leftDragging = false;
};

void StandardInputHandler::mouseMove(double x, double y) {
	// Camera rotating
	if (rightDragging) {
		screen.camera.rotate(screen, Vec3((y - curPos.y) * 0.01, (x - curPos.x) * 0.01, 0), leftDragging);
	}

	if (leftDragging) {
		double speed = 0.01;
		double dmx = (x - curPos.x) * speed;
		double dmy = (y - curPos.y) * -speed;
		
		// Phyiscal moving
		if (screen.selectedPhysical != nullptr) {
			moveGrabbedPhysicalLateral(screen);
		}
	} else {
		screen.world->selectedPhysical = nullptr;
	}

	// Camera moving
	if (middleDragging) {
		screen.camera.move(screen, Vec3((x - curPos.x) * -0.5, (y - curPos.y) * 0.5, 0), leftDragging);
	}

	curPos = Vec2(x, y);
};

void StandardInputHandler::scroll(double xOffset, double yOffset) {
	screen.camera.move(screen, 0, 0, -0.5 * yOffset, leftDragging);
};

void StandardInputHandler::mouseExit() {
	rightDragging = false;
	leftDragging = false;
};
