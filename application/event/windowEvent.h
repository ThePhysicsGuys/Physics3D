#pragma once

#include "event.h"

class WindowEvent : public Event {
public:
	EVENT_CATEGORY(EventCategoryWindow);

protected:
	WindowEvent() {};
};

class WindowResizeEvent : public WindowEvent {
private:
	int width;
	int height;
public:
	EVENT_TYPE(WindowResize);

	WindowResizeEvent(int width, int height) : width(width), height(height) {};
};

class WindowCloseEvent : public WindowEvent {;
public:
	EVENT_TYPE(WindowClose);

	WindowCloseEvent() {};
};