#include "core.h"

#include "standardInputHandler.h"

#include "../engine/input/keyboard.h"

#include "../application/worldBuilder.h"
#include "../engine/options/keyboardOptions.h"
#include "../graphics/renderer.h"
#include <Physics3D/misc/toString.h>
#include "../application.h"
#include "../graphics/gui/gui.h"
#include "../graphics/debug/visualDebug.h"
#include "../graphics/buffers/frameBuffer.h"
#include "../graphics/glfwUtils.h"
#include "../worlds.h"
#include "../view/screen.h"
#include "ecs/components.h"
#include "../view/camera.h"
#include <random>

#include "layer/pickerLayer.h"

namespace P3D::Application {

#define KEY_BIND(name) \
	if (key == name)

#define KEY_RANGE(first, second) \
	if (key > first && key < second)
	
StandardInputHandler::StandardInputHandler(GLFWwindow* window, Screen& screen) : InputHandler(window), screen(screen) {}

void StandardInputHandler::onEvent(Engine::Event& event) {
	using namespace Engine;

	Application::onEvent(event);

	EventDispatcher dispatcher(event);

	if (dispatcher.dispatch<KeyPressEvent>(EVENT_BIND(StandardInputHandler::onKeyPress)))
		return;

	if (dispatcher.dispatch<DoubleKeyPressEvent>(EVENT_BIND(StandardInputHandler::onDoubleKeyPress)))
		return;

	if (dispatcher.dispatch<WindowResizeEvent>(EVENT_BIND(StandardInputHandler::onWindowResize)))
		return;

}

bool StandardInputHandler::onWindowResize(Engine::WindowResizeEvent& event) {
	Vec2i dimension = Vec2i(event.getWidth(), event.getHeight());

	Graphics::Renderer::viewport(Vec2i(), dimension);

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

	KEY_BIND(KeyboardOptions::Tick::Speed::up) {
		setSpeed(getSpeed() * 1.5);
		Log::info("TPS is now: %f", getSpeed());
	}
	
	KEY_BIND(KeyboardOptions::Tick::Speed::down) {
		setSpeed(getSpeed() / 1.5);
		Log::info("TPS is now: %f", getSpeed());
	}

	KEY_BIND(KeyboardOptions::Tick::run) {
		if (isPaused()) runTick();
	}

	KEY_BIND(Keyboard::KEY_O) {
		world.asyncModification([]() {
			Position pos(0.0 + (rand() % 100) * 0.001, 1.0 + (rand() % 100) * 0.001, 0.0 + (rand() % 100) * 0.001);

			WorldBuilder::createDominoAt(GlobalCFrame(pos, Rotation::fromEulerAngles(0.2, 0.3, 0.7)));
		}); 
		Log::info("Created domino! There are %d objects in the world! ", screen.world->getPartCount());
	}

	return true;
}

bool StandardInputHandler::onKeyPress(Engine::KeyPressEvent& event) {
	using namespace Graphics::VisualDebug;
	using namespace Engine;

	Key key = event.getKey();

	KEY_BIND(KeyboardOptions::Tick::pause) {
		togglePause();
	}

	KEY_BIND(KeyboardOptions::Part::remove) {
		if (screen.selectedPart != nullptr) {
			screen.world->asyncModification([world = screen.world, selectedPart = screen.selectedPart]() {
				world->removePart(selectedPart);
			});
			screen.world->selectedPart = nullptr;
			screen.selectedPart = nullptr;
		}
	}

	KEY_BIND(KeyboardOptions::Debug::pies) {
		renderPiesEnabled = !renderPiesEnabled;
	}
	
	KEY_BIND(KeyboardOptions::Part::makeMainPart) {
		Log::info("Made %s the main part of it's physical", screen.registry.getOr<Comp::Name>(screen.selectedPart->entity, "").name.c_str());
		screen.selectedPart->makeMainPart();
	}
	
	KEY_BIND(KeyboardOptions::Part::makeMainPhysical) {
		if (screen.selectedPart) {
			if (screen.selectedPart->parent != nullptr) {
				if (!screen.selectedPart->parent->isMainPhysical()) {
					Log::info("Made %s the main physical", screen.registry.getOr<Comp::Name>(screen.selectedPart->entity, "").name.c_str());
					((ConnectedPhysical*) screen.selectedPart->parent)->makeMainPhysical();
				} else {
					Log::warn("This physical is already the main physical!");
				}
			} else {
				Log::warn("This part has no physical!");
			}
		}
	}
	
	KEY_BIND(KeyboardOptions::World::valid) {
		Log::debug("Checking World::isValid()");
		screen.world->asyncReadOnlyOperation([world = screen.world]() {
			world->isValid();
		});
	}
	
	KEY_BIND(KeyboardOptions::Edit::rotate) {
		PickerLayer::toolManagers[0].selectTool("Rotate");
	}
	
	KEY_BIND(KeyboardOptions::Edit::translate) {
		PickerLayer::toolManagers[0].selectTool("Translate");
	}
	
	KEY_BIND(KeyboardOptions::Edit::scale) {
		PickerLayer::toolManagers[0].selectTool("Scale");
	}

	KEY_BIND(KeyboardOptions::Edit::select) {
		PickerLayer::toolManagers[0].selectTool("Select");
	}

	KEY_BIND(KeyboardOptions::Edit::region) {
		PickerLayer::toolManagers[0].selectTool("Select region");
	}
	
	KEY_BIND(KeyboardOptions::Debug::spheres) {
		colissionSpheresMode = static_cast<SphereColissionRenderMode>((static_cast<int>(colissionSpheresMode) + 1) % 3);
	}
	
	KEY_BIND(KeyboardOptions::Debug::tree) {
		colTreeRenderMode++;
		int worldLayerCount = getMaxLayerID(world.layers); // must be a signed int, because size_t comparison makes the comparison unsigned
		if(colTreeRenderMode >= worldLayerCount) {
			colTreeRenderMode = -2;
		}
	}
	
	KEY_BIND(KeyboardOptions::Application::close) {
		Graphics::GLFW::closeWindow();
	} else if (key == Keyboard::KEY_F11) {
		Graphics::GLFW::swapFullScreen();
	}

	KEY_RANGE(Keyboard::KEY_F1, Keyboard::KEY_F9) {
		toggleVectorType(static_cast<Debug::VectorType>(key.getCode() - Keyboard::KEY_F1.getCode()));
	}

	KEY_RANGE(Keyboard::KEY_NUMBER_1, Keyboard::KEY_NUMBER_3) {
		togglePointType(static_cast<Debug::PointType>(key.getCode() - Keyboard::KEY_NUMBER_1.getCode()));
	}

	return onKeyPressOrRepeat(event);
};

bool StandardInputHandler::onDoubleKeyPress(Engine::DoubleKeyPressEvent& event) {
	using namespace Engine;

	Key key = event.getKey();

	KEY_BIND(KeyboardOptions::Move::fly) {
		toggleFlying();
	}

	return true;
}

};