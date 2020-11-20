#pragma once

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