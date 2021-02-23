#include "core.h"

#include "toolManager.h"

namespace P3D::Engine {
	
	ToolManager::ToolManager(): activeTool(nullptr) {

	}

	ToolManager::~ToolManager() {
		ToolManager::onClose();
	}

	ToolManager& ToolManager::operator=(ToolManager&& other) noexcept {
		if (this != &other) {
			activeTool = std::exchange(other.activeTool, nullptr);
			tools = std::move(other.tools);
		}

		return *this;
	}

	ToolManager::ToolManager(ToolManager&& other) noexcept: activeTool(other.activeTool), tools(std::move(other.tools)) {
		other.activeTool = nullptr;
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

		for (Tool* tool : tools) {
			tool->onDeregister();
			delete tool;
		}

		activeTool = nullptr;
		tools.clear();
	}

	bool ToolManager::deselectTool() {
		if (activeTool == nullptr)
			return false;

		activeTool->onDeselect();
		activeTool = nullptr;

		return true;
	}

	bool ToolManager::selectTool(const std::string& name) {
		auto iterator = std::find_if(tools.begin(), tools.end(),
		                             [&name](Tool* tool) { return tool->getName() == name; });

		if (iterator == tools.end())
			return false;

		Tool* tool = *iterator;

		if (activeTool != nullptr)
			activeTool->onDeselect();

		activeTool = tool;
		activeTool->onSelect();

		return true;
	}

	bool ToolManager::selectTool(Tool* tool) {
		auto iterator = std::find(tools.begin(), tools.end(), tool);
		if (tool == nullptr)
			return deselectTool();

		if (activeTool != nullptr)
			activeTool->onDeselect();

		activeTool = tool;
		activeTool->onSelect();

		return true;
	}

	bool ToolManager::isSelected(Tool* tool) {
		return activeTool == tool;
	}

	bool ToolManager::isSelected(const std::string& name) {
		return activeTool->getName() == name;
	}

	std::vector<Tool*>::iterator ToolManager::begin() {
		return tools.begin();
	}

	std::vector<Tool*>::iterator ToolManager::end() {
		return tools.end();
	}

}
