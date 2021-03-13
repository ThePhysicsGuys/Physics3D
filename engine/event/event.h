#pragma once

#include <string>

namespace P3D::Engine {

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

struct Event {
public:
	bool handled = false;

	[[nodiscard]] virtual EventType getType() const = 0;
	[[nodiscard]] virtual char getCategory() const = 0;
	[[nodiscard]] virtual std::string getName() const = 0;

	[[nodiscard]] bool inCategory(char category) const {
		return (category & getCategory()) != 0;
	}

	[[nodiscard]] bool inCategory(const EventCategory& category) const {
		return inCategory(static_cast<char>(category));
	}
};

#define EVENT_BIND(function) \
	[&] (auto& event) -> bool { return function(event); }
	
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