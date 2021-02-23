#include "core.h"

#include "pickerLayer.h"
#include "view/screen.h"
#include "picker/picker.h"
#include "input/standardInputHandler.h"

#include "../engine/tool/toolManager.h"
#include "../graphics/renderer.h"
#include "../picker/tools/selectionTool.h"
#include "../picker/tools/translationTool.h"

namespace P3D::Application {

std::vector<Engine::ToolManager> PickerLayer::toolManagers;
	
void PickerLayer::onInit(Engine::Registry64& registry) {
	Picker::onInit();

	Engine::ToolManager toolManager;
	toolManager.registerTool<SelectionTool>();
	toolManager.registerTool<TranslationTool>();

	toolManagers.push_back(std::move(toolManager));
}

void PickerLayer::onUpdate(Engine::Registry64& registry) {
	Screen* screen = static_cast<Screen*>(this->ptr);

	// Update selection context
	screen->selectionContext.mouse = handler->mousePosition;
	screen->selectionContext.ray = getMouseRay(*screen, handler->mousePosition);

	Picker::onUpdate(*screen, handler->mousePosition);

	for (Engine::ToolManager& toolManager : toolManagers)
		toolManager.onUpdate();
}

void PickerLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {
	Picker::onEvent(event);

	for (Engine::ToolManager& toolManager : toolManagers)
		toolManager.onEvent(event);
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
	for (Engine::ToolManager& toolManager : toolManagers)
		toolManager.onRender();

	endScene();
}

void PickerLayer::onClose(Engine::Registry64& registry) {
	Picker::onClose();

	for (Engine::ToolManager& toolManager : toolManagers)
		toolManager.onClose();
}

};