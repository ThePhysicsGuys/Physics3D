#include "core.h"

#include "pickerLayer.h"
#include "view/screen.h"
#include "picker/picker.h"
#include "input/standardInputHandler.h"

#include "../engine/tool/toolManager.h"
#include "../graphics/renderer.h"
#include "../graphics/renderer.h"
#include "../picker/selectionTool.h"

namespace P3D::Application {

Engine::ToolManager toolManager;

void PickerLayer::onInit(Engine::Registry64& registry) {
	Picker::onInit();
	
	toolManager.onInit();
	//toolManager.registerTool<SelectionTool>();
	//toolManager.selectTool<SelectionTool>();
}

void PickerLayer::onUpdate(Engine::Registry64& registry) {
	Screen* screen = static_cast<Screen*>(this->ptr);

	Picker::onUpdate(*screen, handler->mousePosition);
	
	toolManager.onUpdate();
}

void PickerLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {
	Picker::onEvent(event);

	toolManager.onEvent(event);
}

void PickerLayer::onRender(Engine::Registry64& registry) {
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

void PickerLayer::onClose(Engine::Registry64& registry) {
	Picker::onClose();

	toolManager.onClose();
}

};