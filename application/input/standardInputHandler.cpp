#include "standardInputHandler.h"

#include "keyboard.h"
#include "mouse.h"

#include "../application/worldBuilder.h"
#include "../options/keyboardOptions.h"
#include "../view/renderUtils.h"
#include "../engine/math/mathUtil.h"
#include "../application.h"
#include "../view/picker/picker.h"
#include "../view/gui/gui.h"
#include "../view/debug/visualDebug.h"
#include "../shapeLibrary.h"

#include <algorithm>
#include <random>

#include "../worlds.h"
#include "../view/screen.h"
#include "../view/camera.h"

StandardInputHandler::StandardInputHandler(GLFWwindow* window, Screen& screen) : InputHandler(window), screen(screen) {}

void StandardInputHandler::framebufferResize(Vec2i dimension) {
	Renderer::viewport(Vec2i(), dimension);

	(*screen.eventHandler.windowResizeHandler) (screen, dimension);
}

void StandardInputHandler::keyDownOrRepeat(int key, int modifiers) {
	if (KeyboardOptions::Tick::Speed::up == key) {
		setSpeed(getSpeed() * 1.5);
		Log::info("TPS is now: %f", getSpeed());
	} else if (KeyboardOptions::Tick::Speed::down == key) {
		setSpeed(getSpeed() / 1.5);
		Log::info("TPS is now: %f", getSpeed());
	} else if (KeyboardOptions::Tick::run == key) {
		runTick();
	} else if (Keyboard::O == key) {
		WorldBuilder::createDominoAt(Position(0.0 + (rand() % 100) * 0.001, 1.0 + (rand() % 100) * 0.001, 0.0 + (rand() % 100) * 0.001), fromEulerAngles(0.2, 0.3, 0.7));
		Log::info("Created domino! There are %d objects in the world! ", screen.world->getPartCount());
	}
}

void StandardInputHandler::keyDown(int key, int modifiers) {
	if (KeyboardOptions::Tick::pause == key) {
		togglePause();
	} else if (KeyboardOptions::Part::remove == key) {
		if (screen.selectedPart != nullptr) {
			screen.world->removePart(screen.selectedPart);
			screen.world->selectedPart = nullptr;
			screen.selectedPart = nullptr;
		}
	} else if (KeyboardOptions::Debug::pies == key) {
		renderPiesEnabled = !renderPiesEnabled;
	} else if (KeyboardOptions::Part::anchor == key) {
		if (screen.selectedPart != nullptr) {
			if (screen.world->isAnchored(screen.selectedPart->parent)) {
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
	} else if (KeyboardOptions::World::valid == key) {
		Log::debug("Checking World::isValid()");
		screen.world->isValid();
	} else if (KeyboardOptions::Edit::rotate == key) {
		Picker::editTools.editMode = EditTools::EditMode::ROTATE;
	} else if (KeyboardOptions::Edit::translate == key) {
			Picker::editTools.editMode = EditTools::EditMode::TRANSLATE;
	} else if (KeyboardOptions::Edit::scale == key) {
			Picker::editTools.editMode = EditTools::EditMode::SCALE;
	} else if (KeyboardOptions::Debug::spheres == key) {
			colissionSpheresMode = static_cast<SphereColissionRenderMode>((static_cast<int>(colissionSpheresMode) + 1) % 3);
	} else if (KeyboardOptions::Debug::tree == key) {
		renderColTree = static_cast<ColTreeRenderMode>((static_cast<int>(renderColTree) + 1) % 3);
	}
	

	if(Keyboard::F1 <= key && Keyboard::F9 >= key) {
		toggleDebugVecType(static_cast<Debug::VectorType>(key - Keyboard::F1.code));
	}
	if (Keyboard::NUMBER_1 <= key && Keyboard::NUMBER_3 >= key) {
		toggleDebugPointType(static_cast<Debug::PointType>(key - Keyboard::NUMBER_1.code));
	}
};

void StandardInputHandler::doubleKeyDown(int key, int modifiers) {
	if(KeyboardOptions::Move::fly == key) {
		toggleFlying();
	}
}

void StandardInputHandler::mouseDown(int button, int mods) {
	if (Mouse::RIGHT == button) rightDragging = true;
	if (Mouse::MIDDLE == button) middleDragging = true;
	if (Mouse::LEFT == button) leftDragging = true;

	if (Mouse::LEFT == button) {
		GUI::selectedComponent = GUI::intersectedComponent;

		if (GUI::intersectedComponent) {
			GUI::select(GUI::superParent(GUI::intersectedComponent));
			GUI::intersectedComponent->press(GUI::map(cursorPosition));
			GUI::intersectedPoint = GUI::map(cursorPosition) - GUI::intersectedComponent->position;
		} else {
			Picker::press(screen);
		}
	}
};

void StandardInputHandler::mouseUp(int button, int mods) {
	if (Mouse::RIGHT == button) rightDragging = false;
	if (Mouse::MIDDLE == button) middleDragging = false;
	if (Mouse::LEFT == button) leftDragging = false;

	if (Mouse::LEFT == button) {
		if (GUI::selectedComponent){
			GUI::selectedComponent->release(GUI::map(cursorPosition));
		}
		
		Picker::release(screen);
	}
};

void StandardInputHandler::mouseMove(double x, double y) {
	double dmx = (x - cursorPosition.x);
	double dmy = (y - cursorPosition.y);
	Vec2 newCursorPosition = Vec2(x, y);
	Vec2 guiCursorPosition = GUI::map(cursorPosition);
	Vec2 newGuiCursorPosition = GUI::map(newCursorPosition);

	if (GUI::intersectedComponent) {
		GUI::intersectedComponent->hover(guiCursorPosition);
	}

	// Camera rotating
	if (rightDragging) {
		screen.camera.rotate(screen, dmy * 0.1, dmx * 0.1, 0, leftDragging);
	}
	
	if (leftDragging) {
		if (GUI::selectedComponent) {
			GUI::selectedComponent->drag(newGuiCursorPosition, guiCursorPosition);
		} else {
			// Phyiscal moving
			Picker::drag(screen);
		}
	}

	// Camera moving
	if (middleDragging) {
		screen.camera.move(screen, dmx * -0.5, dmy * 0.5, 0, leftDragging);
	}

	cursorPosition = newCursorPosition;
};

void StandardInputHandler::scroll(double xOffset, double yOffset) {
	screen.camera.speed = GUI::clamp(screen.camera.speed * (1 + 0.2 * yOffset), 0.001, 100);
};

void StandardInputHandler::mouseExit() {
	rightDragging = false;
	leftDragging = false;
};
