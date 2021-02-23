#pragma once

#include <string>

namespace P3D::Engine {

class Event;

#define DEFINE_TOOL(name, description, cursor) \
	inline std::string getName() { return getStaticName(); } \
	inline std::string getDescription() { return getStaticDescription(); } \
	inline static std::string getStaticName() { return name; } \
	inline static std::string getStaticDescription() { return description; } \
	inline int getCursorType() { return cursor; }

class Tool {

public:
	Tool() = default;

	/*
		Returns the name of this tool
	*/
	virtual std::string getName() = 0;
	static std::string getStaticName();

	/*
		Returns the description of this tool
	*/
	virtual std::string getDescription() = 0;
	static std::string getStaticDescription();

	/*
		When the tool gets registered
	*/
	virtual void onRegister() {}

	/*
		When the tool gets deregistered
	*/
	virtual void onDeregister() {}
	
	/*
		When the tool is selected
	*/
	virtual void onSelect() {}

	/*
		When the tool is deselected
	*/
	virtual void onDeselect() {}

	/*
		When the tool gets updated
	*/
	virtual void onUpdate() {}

	/*
		When the tool gets rendered
	*/
	virtual void onRender() {}

	/*
		When the tool receives an event
	*/
	virtual void onEvent(Event& event) {}
	
};

};