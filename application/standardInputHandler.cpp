#include "standardInputHandler.h"
#include "view/renderUtils.h"
#include "../engine/math/mathUtil.h"
#include "application.h"
#include "view/picker.h"
#include "view/gui/gui.h"
#include "view/visualDebug.h"
#include "objectLibrary.h"
#include <algorithm>
#include <random>

StandardInputHandler::StandardInputHandler(GLFWwindow* window, Screen& screen) : InputHandler(window), screen(screen) {}

void StandardInputHandler::framebufferResize(int width, int height) {
	Vec2i dimension = Vec2i(width, height);
	if (screen.properties.get("include_titlebar_offset") == "true") {
		Vec4i size = Renderer::frameSize();
		dimension.y -= size.y;
	}

	Renderer::viewport(Vec2i(), dimension);

	(*screen.eventHandler.windowResizeHandler) (screen, dimension.x, dimension.y);
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
		Log::info("Created domino! There are %d objects in the world! ", screen.world->getPartCount());
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
				screen.world->selectedPart = nullptr;
				screen.selectedPart = nullptr;
			}
			break;
		case GLFW_KEY_F:
			renderPies = !renderPies;
			break;
		case GLFW_KEY_Q:
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
			break;
		case GLFW_KEY_R:
			Picker::editMode = Picker::EditMode::ROTATE;
			break;
		case GLFW_KEY_E:
			Picker::editMode = Picker::EditMode::TRANSLATE;
			break;
		case GLFW_KEY_C:
			Picker::editMode = Picker::EditMode::SCALE;
			break;
		case GLFW_KEY_4:
			colissionSpheresMode = static_cast<SphereColissionRenderMode>((static_cast<int>(colissionSpheresMode) + 1) % 3);
			break;
		case GLFW_KEY_5:
			renderColTree = static_cast<ColTreeRenderMode>((static_cast<int>(renderColTree) + 1) % 3);
			break;
	}

	if(key >= GLFW_KEY_F1 && key <= GLFW_KEY_F9) {
		toggleDebugVecType(static_cast<Debug::VecType>(key - GLFW_KEY_F1));
	}
	if (key >= GLFW_KEY_1 && key <= GLFW_KEY_3) {
		toggleDebugPointType(static_cast<Debug::PointType>(key - GLFW_KEY_1));
	}
};

void StandardInputHandler::doubleKeyDown(int key, int modifiers) {
	if(key == GLFW_KEY_SPACE) {
		toggleFlying();
	}
}

void StandardInputHandler::mouseDown(int button, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) rightDragging = true;
	if (button == GLFW_MOUSE_BUTTON_MIDDLE) middleDragging = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT) leftDragging = true;

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		GUI::selectedComponent = GUI::intersectedComponent;

		if (GUI::intersectedComponent) {
			GUI::select(GUI::superParent(GUI::intersectedComponent));
			GUI::intersectedComponent->press(GUI::map(cursorPosition));
			GUI::intersectedPoint = GUI::map(cursorPosition) - GUI::intersectedComponent->position;
		} else {
			Picker::press(screen);
			if (screen.selectedPart) {
				Picker::moveGrabbedPhysicalLateral(screen);
			}
		}
	}
};

void StandardInputHandler::mouseUp(int button, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) rightDragging = false;
	if (button == GLFW_MOUSE_BUTTON_MIDDLE) middleDragging = false;
	if (button == GLFW_MOUSE_BUTTON_LEFT) leftDragging = false;

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GUI::selectedComponent){
			GUI::selectedComponent->release(GUI::map(cursorPosition));
		}
		screen.world->selectedPart = nullptr;
	}
};

void StandardInputHandler::mouseMove(double x, double y) {
	if (GUI::intersectedComponent) {
		GUI::intersectedComponent->hover(GUI::map(cursorPosition));
	}

	// Camera rotating
	if (rightDragging) {
		screen.camera.rotate(screen, Vec3((y - cursorPosition.y) * 0.1, (x - cursorPosition.x) * 0.1, 0), leftDragging);
	}
	
	if (leftDragging) {
		double speed = 0.01;
		double dmx = (x - cursorPosition.x) * speed;
		double dmy = (y - cursorPosition.y) * -speed;

		if (GUI::selectedComponent) {
			GUI::selectedComponent->drag(GUI::map(cursorPosition));
		} else {
			// Phyiscal moving
			if (screen.selectedPart) {
				Picker::moveGrabbedPhysicalLateral(screen);
			}
		}
	}

	// Camera moving
	if (middleDragging) {
		screen.camera.move(screen, Vec3((x - cursorPosition.x) * -0.5, (y - cursorPosition.y) * 0.5, 0), leftDragging);
	}

	cursorPosition = Vec2(x, y);
};

void StandardInputHandler::scroll(double xOffset, double yOffset) {
	screen.camera.speed = fmin(fmax(screen.camera.speed * (1 + 0.2 * yOffset), 0.001), 100);
};

void StandardInputHandler::mouseExit() {
	rightDragging = false;
	leftDragging = false;
};
