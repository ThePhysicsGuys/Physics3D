#pragma once

namespace P3D::Engine {

class Event;

#define DEFINE_TOOL(name, description, cursor) \
	static std::string getName() { return name; } \
	static std::string getDescription() { return description; } \
	int getCursorType() { return cursor; }

class Tool {

public:
	inline Tool() {}

	/*
		Returns the name of this tool
	*/
	static std::string getName();

	/*
		Returns the description of this tool
	*/
	static std::string getDescription();

	/*
		When the tool gets registered
	*/
	inline virtual void onRegister() {}

	/*
		When the tool is selected
	*/
	inline virtual void onSelect() {}

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
	inline virtual void onDeselect() {}

	/*
		When the tool gets deregistered
	*/
	inline virtual void onDeregister() {}
};

};