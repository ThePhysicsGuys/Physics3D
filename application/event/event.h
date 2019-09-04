#pragma once

#include <string>

enum class EventType {
	None = 0,
	WindowClose, WindowResize,
	MouseScroll, MouseMove, MousePress, MouseRelease,
	KeyPress, KeyRelease
};

enum EventCategory {
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
	inline virtual EventType getType() { return getStaticType(); } \
	inline virtual std::string getName() { return #type; }

#define EVENT_CATEGORY(category) \
	virtual char getCategory() { return category; }

class Event {
public:
	inline virtual EventType getType() const = 0;	
	inline virtual char getCategory() const = 0;
	inline virtual std::string getName() const = 0;

	inline bool inCategory(EventCategory category) const {
		return static_cast<char>(category) == getCategory();
	}

protected:
	bool handled = false;
};