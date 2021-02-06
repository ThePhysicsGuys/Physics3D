#include "core.h"

#include "pickerLayer.h"
#include "view/screen.h"
#include "picker/picker.h"
#include "input/standardInputHandler.h"

#include "../engine/tool/toolManager.h"
#include "../graphics/renderer.h"
#include "../picker/selectionTool.h"

namespace P3D::Application {

using Engine::ToolManager;
	
void PickerLayer::onInit(Engine::Registry64& registry) {
	Picker::onInit();
	
	ToolManager::onInit();
	ToolManager::registerTool<SelectionTool>();
	// ToolManager::selectTool<SelectionTool>();
}

void PickerLayer::onUpdate(Engine::Registry64& registry) {
	Screen* screen = static_cast<Screen*>(this->ptr);

	Picker::onUpdate(*screen, handler->mousePosition);
	
	ToolManager::onUpdate();
}

void PickerLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {
	Picker::onEvent(event);

	ToolManager::onEvent(event);
}

void PickerLayer::onRender(Engine::Registry64& registry) {
	using namespace Graphics;
	using namespace Renderer;
	Screen* screen = static_cast<Screen*>(this->ptr);

	beginScene();

	clearDepth();
	enableDepthTest();
	Picker::onRender(*screen);

	disableDepthTest();
	ToolManager::onRender();

	endScene();
}

void PickerLayer::onClose(Engine::Registry64& registry) {
	Picker::onClose();

	ToolManager::onClose();
}

};