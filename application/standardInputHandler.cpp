#include "standardInputHandler.h"
#include "../engine/math/mathUtil.h"
#include "application.h"
#include "gui/picker.h"
#include "gui/form/gui.h"
#include "objectLibrary.h"
#include <algorithm>
#include <random>

StandardInputHandler::StandardInputHandler(GLFWwindow* window, Screen& screen) : InputHandler(window), screen(screen) {}

void StandardInputHandler::framebufferResize(int width, int height) {
	if (screen.properties.get("include_titlebar_offset") == "true") {
		int t, top;
		glfwGetWindowFrameSize(window, &t, &top, &t, &t);
		height = height - top;
	}
	glViewport(0, 0, width, height);
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
		createDominoAt(Vec3(0.0 + (rand() % 100) * 0.001, 1.0 + (rand() % 100) * 0.001, 0.0 + (rand() % 100) * 0.001), fromEulerAngles(0.2, 0.3, 0.7));
		Log::info("Created domino! There are %d objects in the world! ", screen.world->partCount);
		break;
	}
}

void StandardInputHandler::keyDown(int key, int modifiers) {
	switch (key) {
		case GLFW_KEY_P:
			togglePause();
			break;
		case GLFW_KEY_DELETE:
			if (screen.selectedPart != nullptr) {
				screen.world->removePart(screen.selectedPart);
				screen.world->selectedPhysical = nullptr;
				screen.selectedPart = nullptr;
			}
			break;
		case GLFW_KEY_F8:
			renderPies = !renderPies;
			break;
		case GLFW_KEY_F9:
			if(screen.selectedPart != nullptr) {
				if(screen.world->isAnchored(screen.selectedPart->parent)) {
					screen.world->unanchor(screen.selectedPart->parent);
				} else {
					Physical* parent = screen.selectedPart->parent;
					parent->velocity = Vec3();
					parent->angularVelocity = Vec3();
					parent->totalForce = Vec3();
					parent->totalMoment = Vec3();
					screen.world->anchor(screen.selectedPart->parent);
				}
			}
			break;
		case GLFW_KEY_V:
			Log::debug("Checking World::isValid()");
			screen.world->isValid();
	}

	if(key >= GLFW_KEY_F1 && key <= GLFW_KEY_F7) {
		screen.toggleDebugVecType(static_cast<Debug::VecType>(key - GLFW_KEY_F1));
	}
};

void StandardInputHandler::doubleKeyDown(int key, int modifiers) {
	if(key == GLFW_KEY_SPACE) {
		toggleFlying();
	}
}

void StandardInputHandler::keyUp(int key, int modifiers) {};
void StandardInputHandler::keyRepeat(int key, int modifiers) {};
// void StandardInputHandler::mouseEnter() {};

void StandardInputHandler::mouseDown(int button, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) rightDragging = true;
	if (button == GLFW_MOUSE_BUTTON_MIDDLE) middleDragging = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT) leftDragging = true;

	if (GUI::intersectedComponent) {
		GUI::intersectedComponent->click(cursorPosition.x, cursorPosition.y);
	} else {
		(*screen.eventHandler.partClickHandler) (screen, screen.intersectedPart, screen.intersectedPoint);
		if (screen.intersectedPart) {
			screen.world->localSelectedPoint = screen.selectedPart->cframe.globalToLocal(screen.intersectedPoint);
		}
	}
};

void StandardInputHandler::mouseUp(int button, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) rightDragging = false;
	if (button == GLFW_MOUSE_BUTTON_MIDDLE) middleDragging = false;
	if (button == GLFW_MOUSE_BUTTON_LEFT) leftDragging = false;
};

void StandardInputHandler::mouseMove(double x, double y) {
	if (GUI::intersectedComponent) {
		GUI::intersectedComponent->hover(x, y);
	}

	// Camera rotating
	if (rightDragging) {
		screen.camera.rotate(screen, Vec3((y - cursorPosition.y) * 0.01, (x - cursorPosition.x) * 0.01, 0), leftDragging);
	}

	if (leftDragging) {
		double speed = 0.01;
		double dmx = (x - cursorPosition.x) * speed;
		double dmy = (y - cursorPosition.y) * -speed;

		if (GUI::selectedComponent) {
			GUI::selectedComponent->drag(dmx, dmy);
		} else {
			// Phyiscal moving
			if (screen.selectedPart) {
				moveGrabbedPhysicalLateral(screen);
			}
		}
	} else {
		screen.world->selectedPhysical = nullptr;
	}

	// Camera moving
	if (middleDragging) {
		screen.camera.move(screen, Vec3((x - cursorPosition.x) * -0.5, (y - cursorPosition.y) * 0.5, 0), leftDragging);
	}

	cursorPosition = Vec2(x, y);
};

void StandardInputHandler::scroll(double xOffset, double yOffset) {
	screen.camera.move(screen, 0, 0, -0.5 * yOffset, leftDragging);
};

void StandardInputHandler::mouseExit() {
	rightDragging = false;
	leftDragging = false;
};
