#pragma once

#include "../engine/event/mouseEvent.h"

namespace Engine {

#define DEFINE_TOOL(name, description, cursor) \
	std::string getName() { return name; } \
	std::string getDescription() { return description; } \
	int getCursorType() { return cursor; }

class Tool {

public:
	inline Tool() {}

	/*
		Returns the name of this tool
	*/
	virtual std::string getName() = 0;

	/*
		Returns the description of this tool
	*/
	virtual std::string getDescription() = 0;

	/*
		When the tool gets registered
	*/
	inline virtual void onInit() {}

	/*
		When the tool is selected
	*/
	inline virtual void onSetUp() {}

	/*
		When the tool gets updated
	*/
	inline virtual void onUpdate() {}

	/*
		When the tool gets rendered
	*/
	inline virtual void onRender() {}

	/*
		When the tool receives an event
	*/
	inline virtual void onEvent(Event& event) {}

	/*
		Whe the tool is deselected
	*/
	inline virtual void onSetDown() {}

	/*
		When the tool gets uregistered
	*/
	inline virtual void onClose() {}
};

};