#include "core.h"

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
#include "../view/gui/guiUtils.h"
#include "../view/debug/visualDebug.h"
#include "../shapeLibrary.h"
#include "../worlds.h"
#include "../view/screen.h"
#include "../view/camera.h"
#include <random>

StandardInputHandler::StandardInputHandler(GLFWwindow* window, Screen& screen) : InputHandler(window), screen(screen) {}

void StandardInputHandler::onEvent(Event& event) {
	screen.onEvent(event);

	EventDispatcher dispatcher(event);

	if (dispatcher.dispatch<MouseMoveEvent>(BIND_EVENT(StandardInputHandler::onMouseMove)))
		return;
		
	if (dispatcher.dispatch<KeyPressEvent>(BIND_EVENT(StandardInputHandler::onKeyPress)))
		return;	
	
	if (dispatcher.dispatch<MouseReleaseEvent>(BIND_EVENT(StandardInputHandler::onMouseRelease)))
		return;	
	
	if (dispatcher.dispatch<MousePressEvent>(BIND_EVENT(StandardInputHandler::onMousePress)))
		return;

	if (dispatcher.dispatch<MouseScrollEvent>(BIND_EVENT(StandardInputHandler::onMouseScroll)))
		return;

	if (dispatcher.dispatch<DoubleKeyPressEvent>(BIND_EVENT(StandardInputHandler::onDoubleKeyPress)))
		return;

	if (dispatcher.dispatch<MouseExitEvent>(BIND_EVENT(StandardInputHandler::onMouseExit)))
		return;

	if (dispatcher.dispatch<FrameBufferResizeEvent>(BIND_EVENT(StandardInputHandler::onFrameBufferResize)))
		return;

}

bool StandardInputHandler::onFrameBufferResize(FrameBufferResizeEvent& event) {
	Vec2i dimension = Vec2i(event.getWidth(), event.getHeight());

	Renderer::viewport(Vec2i(), dimension);

	(*screen.eventHandler.windowResizeHandler) (screen, dimension);

	return true;
}

bool StandardInputHandler::onKeyPressOrRepeat(KeyPressEvent& event) {
	int key = event.getKey();

	if (KeyboardOptions::Tick::Speed::up == key) {
		setSpeed(getSpeed() * 1.5);
		Log::info("TPS is now: %f", getSpeed());
	} else if (KeyboardOptions::Tick::Speed::down == key) {
		setSpeed(getSpeed() / 1.5);
		Log::info("TPS is now: %f", getSpeed());
	} else if (KeyboardOptions::Tick::run == key) {
		if(isPaused()) runTick();
	} else if (Keyboard::O == key) {
		WorldBuilder::createDominoAt(Position(0.0 + (rand() % 100) * 0.001, 1.0 + (rand() % 100) * 0.001, 0.0 + (rand() % 100) * 0.001), fromEulerAngles(0.2, 0.3, 0.7));
		Log::info("Created domino! There are %d objects in the world! ", screen.world->getPartCount());
	}

	return true;
}

bool StandardInputHandler::onKeyPress(KeyPressEvent& event) {
	if (event.isRepeated()) {
		return onKeyPressOrRepeat(event);
	}

	int key = event.getKey();

	if (KeyboardOptions::Tick::pause == key) {
		togglePause();
	} else if (KeyboardOptions::Part::remove == key) {
		if (screen.selectedPart != nullptr) {
			screen.world->removePart(screen.selectedPart);
			screen.world->selectedPart = nullptr;
			screen.selectedPart = nullptr;
		}
	} else if (KeyboardOptions::Debug::pies == key) {
		Debug::renderPiesEnabled = !Debug::renderPiesEnabled;
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
		Debug::colissionSpheresMode = static_cast<Debug::SphereColissionRenderMode>((static_cast<int>(Debug::colissionSpheresMode) + 1) % 3);
	} else if (KeyboardOptions::Debug::tree == key) {
		Debug::colTreeRenderMode = static_cast<Debug::ColTreeRenderMode>((static_cast<int>(Debug::colTreeRenderMode) + 1) % 3);
	}
	

	if(Keyboard::F1 <= key && Keyboard::F9 >= key) {
		toggleVectorType(static_cast<Debug::VectorType>(key - Keyboard::F1.code));
	}
	if (Keyboard::NUMBER_1 <= key && Keyboard::NUMBER_3 >= key) {
		togglePointType(static_cast<Debug::PointType>(key - Keyboard::NUMBER_1.code));
	}

	return true;
};

bool StandardInputHandler::onDoubleKeyPress(DoubleKeyPressEvent& event) {
	int key = event.getKey();

	if (KeyboardOptions::Move::fly == key) {
		toggleFlying();
	}

	return true;
}

bool StandardInputHandler::onMousePress(MousePressEvent& event) {
	int button = event.getButton();

	if (Mouse::RIGHT == button) rightDragging = true;
	if (Mouse::MIDDLE == button) middleDragging = true;
	if (Mouse::LEFT == button) leftDragging = true;

	if (Mouse::LEFT == button) {
		GUI::selectedComponent = GUI::intersectedComponent;

		if (GUI::intersectedComponent) {
			GUI::select(GUI::superParent(GUI::intersectedComponent));
			GUI::intersectedComponent->press(GUI::map(mousePosition));
			GUI::intersectedPoint = GUI::map(mousePosition) - GUI::intersectedComponent->position;
		} else {
			Picker::press(screen);
		}
	}

	return true;
};

bool StandardInputHandler::onMouseRelease(MouseReleaseEvent& event) {
	int button = event.getButton();

	if (Mouse::RIGHT == button) rightDragging = false;
	if (Mouse::MIDDLE == button) middleDragging = false;
	if (Mouse::LEFT == button) leftDragging = false;

	if (Mouse::LEFT == button) {
		if (GUI::selectedComponent){
			GUI::selectedComponent->release(GUI::map(mousePosition));
		}
		
		Picker::release(screen);
	}

	return true;
};

bool StandardInputHandler::onMouseMove(MouseMoveEvent& event) {
	double dmx = (event.getX() - mousePosition.x);
	double dmy = (event.getY() - mousePosition.y);
	Vec2 newCursorPosition = Vec2(event.getX(), event.getY());
	Vec2 guiCursorPosition = GUI::map(mousePosition);
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

	mousePosition = newCursorPosition;

	return true;
};

bool StandardInputHandler::onMouseScroll(MouseScrollEvent& event) {
	screen.camera.speed = GUI::clamp(screen.camera.speed * (1 + 0.2 * event.getYOffset()), 0.001, 100);

	return true;
};

bool StandardInputHandler::onMouseExit(MouseExitEvent& event) {
	rightDragging = false;
	leftDragging = false;

	return true;
};
