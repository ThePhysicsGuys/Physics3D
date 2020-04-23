#pragma once

#include <string>

namespace Engine {

enum class EventType {
	None = 0,
	WindowClose, WindowResize, WindowDrop, FrameBufferResize,
	MouseScroll, MouseMove, MouseDrag, MousePress, MouseRelease, MouseExit, MouseEnter,
	KeyPress, KeyRelease, KeyDoublePress
};

enum EventCategory : char {
	EventCategoryNone		 = 0 << 0,
	EventCategoryInput		 = 1 << 0,
	EventCategoryMouse		 = 1 << 1, 
	EventCategoryMouseButton = 1 << 2, 
	EventCategoryKeyboard	 = 1 << 3,
	EventCategoryWindow		 = 1 << 4,
	EventCategoryApplication = 1 << 5
};

#define EVENT_TYPE(type) \
	inline static EventType getStaticType() { return EventType::type; } \
	inline virtual EventType getType() const override { return getStaticType(); } \
	inline virtual std::string getName() const override { return #type; }

#define EVENT_CATEGORY(category) \
	virtual char getCategory() const override { return category; }

class Event {
public:
	bool handled = false;

	virtual EventType getType() const = 0;	
	virtual char getCategory() const = 0;
	virtual std::string getName() const = 0;

	inline bool inCategory(char category) const {
		return (category & getCategory()) != 0;
	}

	inline bool inCategory(const EventCategory& category) const {
		return inCategory(category);
	}
};

// For class methods only

#define BIND_EVENT_METHOD(function) \
	std::bind(&function, this, std::placeholders::_1)

#define DISPATCH_EVENT_METHOD(event, type, function) \
	{ EventDispatcher dispatch(event); \
	dispatch.dispatch<type>(BIND_EVENT_METHOD(function)); }

// For free functions only

#define DISPATCH_EVENT_FUNCTION(event, type, function) \
	{ EventDispatcher dispatch(event); \
	dispatch.dispatch<type>(function); }

class EventDispatcher {
private:
	Event& event;

public:
	EventDispatcher(Event& event) : event(event) {}

	template<typename T, typename F>
	bool dispatch(const F& function) {
		if (event.getType() == T::getStaticType()) {
			event.handled = function(static_cast<T&>(event)); 
			return true;
		}
		return false;
	}
};

};