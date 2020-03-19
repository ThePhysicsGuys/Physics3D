#include "core.h"

#include "pickerLayer.h"
#include "view/screen.h"
#include "picker/picker.h"
#include "input/standardInputHandler.h"

#include "../graphics/renderer.h"
#include "../graphics/renderer.h"

namespace Application {

void PickerLayer::onInit() {
	Picker::onInit();
}

void PickerLayer::onUpdate() {
	Screen* screen = static_cast<Screen*>(this->ptr);

	Picker::onUpdate(*screen, handler->mousePosition);
}

void PickerLayer::onEvent(Event& event) {
	Picker::onEvent(event);
}

void PickerLayer::onRender() {
	using namespace Graphics;
	using namespace Graphics::Renderer;
	Screen* screen = static_cast<Screen*>(this->ptr);

	beginScene();

	clearDepth();
	enableDepthTest();

	Picker::onRender(*screen);

	endScene();
}

void PickerLayer::onClose() {
	Picker::onClose();
}

};