#include "core.h"

#include "toolManager.h"
#include "event/event.h"

namespace P3D::Engine {
	
Tool* ToolManager::activeTool = nullptr;
std::map<std::string, Tool*> ToolManager::tools;

	
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

	for (auto& [name, tool] : tools) {
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

bool ToolManager::selectTool(Tool* tool) {
	if (tool == nullptr)
		return deselectTool();

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

bool ToolManager::isSelected(Tool* tool) {
	return activeTool == tool;
}

bool ToolManager::isSelected(const std::string& name) {
	return activeTool->getName() == name;
}

}
