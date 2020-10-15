#pragma once

#include "tool.h"

namespace P3D::Engine {

class ToolManager {
private:
	static Tool* activeTool;
	static std::map<std::string, Tool*> tools;

public:
	template<typename T, typename... Args>
	static bool registerTool(Args&&... args) {
		auto iterator = tools.find(T::getName());
		if (iterator != tools.end())
			return false;

		T* tool = new T(std::forward<Args>(args)...);
		tool->onRegister();
		tools.insert({ T::getName(), tool });

		return true;
	}

	template<typename T>
	static bool deregisterTool() {
		auto iterator = tools.find(T::getName());
		if (iterator == tools.end())
			return false;

		T* tool = iterator->second;
		tools.erase(iterator);
		tool->onDeregister();
		delete tool;
		
		return true;
	}

	static void onInit();
	static void onEvent(Event& event);
	static void onUpdate();
	static void onRender();
	static void onClose();

	static bool deselectTool();
	static bool selectTool(const std::string& name);
	template<typename T>
	static bool selectTool() { 
		return selectTool(T::getName());
	}

};

};