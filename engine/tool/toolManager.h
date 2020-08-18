#pragma once

#include "tool.h"

namespace P3D::Engine {

class ToolManager {
private:
	Tool* activeTool;

	std::map<std::string, Tool*> tools;

public:
	template<typename T, typename... Args>
	bool registerTool(Args... args) {
		auto iterator = tools.find(T::getName());
		if (iterator != tools.end())
			return false;

		T* tool = new T(args...);
		tool->onRegister();
		tools.insert({ T::getName(), tool });

		return true;
	}

	template<typename T>
	bool deregisterTool() {
		auto iterator = tools.find(T::getName());
		if (iterator == tools.end())
			return false;

		T* tool = iterator->second;
		tools.erase(iterator);
		tool->onDeregister();
		delete tool;
		
		return true;
	}

	void onInit();
	void onEvent(Event& event);
	void onUpdate();
	void onRender();
	void onClose();

	bool selectTool(const std::string& name);

	template<typename T>
	bool selectTool() { 
		return selectTool(T::getName());
	};

	bool deselectTool();
};

};