#include "core.h"

#include "toolManager.h"

namespace P3D::Engine {

void ToolManager::onInit() {

}

void ToolManager::onEvent(Event& event) {
	if (activeTool == nullptr)
		return;

	activeTool->onEvent(event);
}

void ToolManager::onUpdate() {
	if (activeTool == nullptr)
		return;

	activeTool->onUpdate();
}

void ToolManager::onRender() {
	if (activeTool == nullptr)
		return;

	activeTool->onRender();
}

void ToolManager::onClose() {
	if (activeTool != nullptr)
		activeTool->onDeselect();

	for (auto iterator : tools) {
		Tool* tool = iterator.second;

		tool->onDeregister();
		delete tool;
	}

	activeTool = nullptr;
	tools.clear();
}

bool ToolManager::selectTool(const std::string& name) {
	auto iterator = tools.find(name);
	if (iterator == tools.end())
		return false;

	Tool* tool = iterator->second;

	if (activeTool != nullptr)
		activeTool->onDeselect();

	activeTool = tool;
	activeTool->onSelect();

	return true;
}

bool ToolManager::deselectTool() {
	if (activeTool == nullptr)
		return false;

	activeTool->onDeselect();
	activeTool = nullptr;

	return true;
}

}