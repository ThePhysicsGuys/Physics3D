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
	unsigned int width;
	unsigned int height;
public:
	EVENT_TYPE(WindowResize);

	WindowResizeEvent(unsigned int width, unsigned int height) : width(width), height(height) {};

	inline unsigned int getWidth() const {
		return width;
	}

	inline unsigned int getHeight() const {
		return height;
	}
};

class WindowCloseEvent : public WindowEvent {;
public:
	EVENT_TYPE(WindowClose);

	WindowCloseEvent() {};
};

class FrameBufferResizeEvent : public WindowEvent {
private:
	unsigned int width;
	unsigned int height;
public:
	EVENT_TYPE(FrameBufferResize);

	FrameBufferResizeEvent(unsigned int width, unsigned int height) : width(width), height(height) {};

	inline unsigned int getWidth() const {
		return width;
	}

	inline unsigned int getHeight() const {
		return height;
	}
};
