#include "core.h"

#include "standardInputHandler.h"

#include "../engine/input/keyboard.h"
#include "../engine/input/mouse.h"

#include "../application/worldBuilder.h"
#include "../engine/options/keyboardOptions.h"
#include "../graphics/renderer.h"
#include "../physics/misc/toString.h"
#include "../application.h"
#include "../picker/picker.h"
#include "../graphics/gui/gui.h"
#include "../graphics/gui/guiUtils.h"
#include "../graphics/debug/visualDebug.h"
#include "../graphics/buffers/frameBuffer.h"
#include "../graphics/glfwUtils.h"
#include "../worlds.h"
#include "../view/screen.h"
#include "../view/camera.h"
#include <random>

namespace P3D::Application {

StandardInputHandler::StandardInputHandler(GLFWwindow* window, Screen& screen) : InputHandler(window), screen(screen) {}

void StandardInputHandler::onEvent(Engine::Event& event) {
	using namespace Engine;

	Application::onEvent(event);

	EventDispatcher dispatcher(event);

	if (dispatcher.dispatch<KeyPressEvent>(BIND_EVENT_METHOD(StandardInputHandler::onKeyPress)))
		return;

	if (dispatcher.dispatch<DoubleKeyPressEvent>(BIND_EVENT_METHOD(StandardInputHandler::onDoubleKeyPress)))
		return;

	if (dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_METHOD(StandardInputHandler::onWindowResize)))
		return;

}

bool StandardInputHandler::onWindowResize(Engine::WindowResizeEvent& event) {
	Vec2i dimension = Vec2i(event.getWidth(), event.getHeight());

	Graphics::Renderer::viewport(Vec2i(), dimension);
	Log::debug("Window resize: %s", str(dimension).c_str());
	(*screen.eventHandler.windowResizeHandler) (screen, dimension);

	return true;
}

bool StandardInputHandler::onFrameBufferResize(Engine::FrameBufferResizeEvent& event) {
	Vec2i dimension = Vec2i(event.getWidth(), event.getHeight());

	Graphics::Renderer::viewport(Vec2i(), dimension);
	
	float aspect = float(dimension.x) / float(dimension.y);
	screen.camera.onUpdate(aspect);
	screen.dimension = dimension;
	screen.screenFrameBuffer->resize(screen.dimension);

	Graphics::GUI::windowInfo.aspect = aspect;
	Graphics::GUI::windowInfo.dimension = dimension;

	(*screen.eventHandler.windowResizeHandler) (screen, dimension);

	return true;
}

bool StandardInputHandler::onKeyPressOrRepeat(Engine::KeyPressEvent& event) {
	using namespace Engine;

	Key key = event.getKey();

	if (key == KeyboardOptions::Tick::Speed::up) {
		setSpeed(getSpeed() * 1.5);
		Log::info("TPS is now: %f", getSpeed());
	} else if (key == KeyboardOptions::Tick::Speed::down) {
		setSpeed(getSpeed() / 1.5);
		Log::info("TPS is now: %f", getSpeed());
	} else if (key == KeyboardOptions::Tick::run) {
		if (isPaused()) runTick();
	} else if (key == Keyboard::KEY_O) {
		world.asyncModification([]() {
			Position pos(0.0 + (rand() % 100) * 0.001, 1.0 + (rand() % 100) * 0.001, 0.0 + (rand() % 100) * 0.001);

			WorldBuilder::createDominoAt(GlobalCFrame(pos, Rotation::fromEulerAngles(0.2, 0.3, 0.7)));
		}); 
		Log::info("Created domino! There are %d objects in the world! ", screen.world->getPartCount());
	}

	return true;
}

bool StandardInputHandler::onKeyPress(Engine::KeyPressEvent& event) {
	using namespace Graphics::Debug;
	using namespace Engine;

	Key key = event.getKey();

	if (key == KeyboardOptions::Tick::pause) {
		togglePause();
	} else if (key == KeyboardOptions::Part::remove) {
		if (screen.selectedPart != nullptr) {
			screen.world->asyncModification([world = screen.world, selectedPart = screen.selectedPart]() {
				world->removePart(selectedPart);
			});
			screen.world->selectedPart = nullptr;
			screen.selectedPart = nullptr;
		}
	} else if (key == KeyboardOptions::Debug::pies) {
		renderPiesEnabled = !renderPiesEnabled;
	} else if(key == KeyboardOptions::Part::makeMainPart) {
		Log::info("Made %s the main part of it's physical", screen.selectedPart->name.c_str());
		screen.selectedPart->makeMainPart();
	} else if(key == KeyboardOptions::Part::makeMainPhysical) {
		if (screen.selectedPart) {
			if (screen.selectedPart->parent != nullptr) {
				if (!screen.selectedPart->parent->isMainPhysical()) {
					Log::info("Made %s the main physical", screen.selectedPart->name.c_str());
					((ConnectedPhysical*) screen.selectedPart->parent)->makeMainPhysical();
				} else {
					Log::warn("This physical is already the main physical!");
				}
			} else {
				Log::warn("This part has no physical!");
			}
		}
	} else if (key == KeyboardOptions::World::valid) {
		Log::debug("Checking World::isValid()");
		screen.world->asyncReadOnlyOperation([world = screen.world]() {
			world->isValid();
		});
	} else if (key == KeyboardOptions::Edit::rotate) {
		Picker::editTools.editMode = EditTools::EditMode::ROTATE;
	} else if (key == KeyboardOptions::Edit::translate) {
		Picker::editTools.editMode = EditTools::EditMode::TRANSLATE;
	} else if (key == KeyboardOptions::Edit::scale) {
		Picker::editTools.editMode = EditTools::EditMode::SCALE;
	} else if (key == KeyboardOptions::Debug::spheres) {
		colissionSpheresMode = static_cast<SphereColissionRenderMode>((static_cast<int>(colissionSpheresMode) + 1) % 3);
	} else if (key == KeyboardOptions::Debug::tree) {
		colTreeRenderMode = static_cast<ColTreeRenderMode>((static_cast<int>(colTreeRenderMode) + 1) % 5);
	} else if (key == KeyboardOptions::Application::close) {
		Graphics::GLFW::closeWindow();
	}

	if (key < Keyboard::KEY_F9 && key > Keyboard::KEY_F1) {
		toggleVectorType(static_cast<Debug::VectorType>(key.getCode() - Keyboard::KEY_F1.getCode()));
	}

	if (key < Keyboard::KEY_NUMBER_3 && key > Keyboard::KEY_NUMBER_1) {
		togglePointType(static_cast<Debug::PointType>(key.getCode() - Keyboard::KEY_NUMBER_1.getCode()));
	}

	return onKeyPressOrRepeat(event);
};

bool StandardInputHandler::onDoubleKeyPress(Engine::DoubleKeyPressEvent& event) {
	using namespace Engine;

	Key key = event.getKey();

	if (key == KeyboardOptions::Move::fly) {
		toggleFlying();
	}

	return true;
}

};