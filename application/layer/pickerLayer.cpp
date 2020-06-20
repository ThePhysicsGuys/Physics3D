#include "core.h"

#include "pickerLayer.h"
#include "view/screen.h"
#include "picker/picker.h"
#include "input/standardInputHandler.h"

#include "../engine/tool/toolManager.h"
#include "../graphics/renderer.h"
#include "../graphics/renderer.h"
#include "../picker/selectionTool.h"

namespace Application {

Engine::ToolManager toolManager;

void PickerLayer::onInit() {
	Picker::onInit();
	
	toolManager.onInit();
	//toolManager.registerTool<SelectionTool>();
	//toolManager.selectTool<SelectionTool>();
}

void PickerLayer::onUpdate() {
	Screen* screen = static_cast<Screen*>(this->ptr);

	Picker::onUpdate(*screen, handler->mousePosition);
	
	toolManager.onUpdate();
}

void PickerLayer::onEvent(Engine::Event& event) {
	Picker::onEvent(event);

	toolManager.onEvent(event);
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
	toolManager.onRender();

	endScene();
}

void PickerLayer::onClose() {
	Picker::onClose();

	toolManager.onClose();
}

};