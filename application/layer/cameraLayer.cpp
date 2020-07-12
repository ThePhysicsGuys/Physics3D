#include "core.h"

#include "cameraLayer.h"

#include "../view/screen.h"
#include "../application/input/standardInputHandler.h"
#include "../engine/options/keyboardOptions.h"

namespace Application {

void CameraLayer::onInit() {
	Screen* screen = static_cast<Screen*>(this->ptr);

	// Camera init
	screen->camera.setPosition(Position(1.0, 2.0, 3.0));
	screen->camera.setRotation(Vec3(0, 3.1415, 0.0));
	screen->camera.onUpdate(1.0f, screen->camera.aspect, 0.01f, 10000.0f);
}

void CameraLayer::onUpdate() {
	Screen* screen = static_cast<Screen*>(this->ptr);

	std::chrono::time_point<std::chrono::steady_clock> curUpdate = std::chrono::steady_clock::now();
	std::chrono::nanoseconds deltaTnanos = curUpdate - this->lastUpdate;
	this->lastUpdate = curUpdate;

	double speedAdjustment = deltaTnanos.count() * 0.000000001 * 60.0;

	// IO events
	if (handler->anyKey) {
		bool leftDragging = handler->leftDragging;
		if (handler->getKey(KeyboardOptions::Move::forward))
			screen->camera.move(*screen, 0, 0, -1 * speedAdjustment, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::backward))
			screen->camera.move(*screen, 0, 0, 1 * speedAdjustment, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::right))
			screen->camera.move(*screen, 1 * speedAdjustment, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::left))
			screen->camera.move(*screen, -1 * speedAdjustment, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::ascend))
			if (screen->camera.flying)
				screen->camera.move(*screen, 0, 1 * speedAdjustment, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::descend))
			if (screen->camera.flying)
				screen->camera.move(*screen, 0, -1 * speedAdjustment, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::left))
			screen->camera.rotate(*screen, 0, 1 * speedAdjustment, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::right))
			screen->camera.rotate(*screen, 0, -1 * speedAdjustment, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::up))
			screen->camera.rotate(*screen, 1 * speedAdjustment, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::down))
			screen->camera.rotate(*screen, -1 * speedAdjustment, 0, 0, leftDragging);
	}

	screen->camera.onUpdate();
}

void CameraLayer::onEvent(Engine::Event& event) {
	Screen* screen = static_cast<Screen*>(this->ptr);
	screen->camera.onEvent(event);
}

void CameraLayer::onRender() {
	Screen* screen = static_cast<Screen*>(this->ptr);

}

void CameraLayer::onClose() {

}
};
