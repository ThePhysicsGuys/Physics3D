#include "core.h"

#include "pickerLayer.h"
#include "view/screen.h"
#include "picker/picker.h"
#include "input/standardInputHandler.h"

#include "../graphics/renderer.h"
#include "../graphics/renderer.h"
#include "../picker/selectionTool.h"

namespace Application {

SelectionTool tool;

void PickerLayer::onInit() {
	Picker::onInit();
	tool.onInit();
}

void PickerLayer::onUpdate() {
	Screen* screen = static_cast<Screen*>(this->ptr);

	Picker::onUpdate(*screen, handler->mousePosition);
	tool.onUpdate();
}

void PickerLayer::onEvent(Engine::Event& event) {
	Picker::onEvent(event);
	tool.onEvent(event);
}

void PickerLayer::onRender() {
	using namespace Graphics;
	using namespace Graphics::Renderer;
	Screen* screen = static_cast<Screen*>(this->ptr);

	beginScene();

	clearDepth();
	enableDepthTest();

	Picker::onRender(*screen);

	disableDepthTest();

	tool.onRender();

	endScene();
}

void PickerLayer::onClose() {
	Picker::onClose();
	tool.onClose();
}

};