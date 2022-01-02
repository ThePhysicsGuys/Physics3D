#pragma once

#include "tool.h"

namespace P3D::Engine {

class Event;

class ToolManager {
public:
	Tool* activeTool;
	std::vector<Tool*> tools;

	ToolManager();
	virtual ~ToolManager();

	ToolManager(ToolManager&& other) noexcept;
	ToolManager& operator=(ToolManager&& other) noexcept;

	ToolManager(const ToolManager& other) = delete;
	ToolManager& operator=(const ToolManager& other) = delete;

	template<typename DerivedTool, typename... Args>
	std::enable_if_t<std::is_base_of_v<Tool, DerivedTool>, bool> registerTool(Args&&... args) {
		DerivedTool* tool = new DerivedTool(std::forward<Args>(args)...);
		tools.push_back(tool);
		tool->onRegister();
		
		return true;
	}

	template<typename DerivedTool>
	std::enable_if_t<std::is_base_of_v<Tool, DerivedTool>, bool> deregisterTool() {
		auto iterator = std::find_if(tools.begin(), tools.end(), [] (Tool* tool) { return tool->getName() == DerivedTool::getStaticName(); });
		if (iterator == tools.end())
			return false;

		DerivedTool* tool = *iterator;
		
		tools.erase(iterator);
		tool->onDeregister();
		delete tool;
		
		return true;
	}

	virtual void onEvent(Event& event);
	virtual void onUpdate();
	virtual void onRender();
	virtual void onClose();

	bool deselectTool();

	bool selectTool(const std::string& name);
	bool selectTool(Tool* tool);

	bool isSelected(Tool* tool);
	bool isSelected(const std::string& name);

	template<typename T>
	bool selectTool() {
		return selectTool(T::getStaticName());
	}
	
	template<typename T>
	bool isSelected() {
		return isSelected(T::getStaticName());
	}

	std::vector<Tool*>::iterator begin();
	std::vector<Tool*>::iterator end();
};

};