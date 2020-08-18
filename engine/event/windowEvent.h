#pragma once

#include "event.h"

namespace P3D::Engine {

class WindowEvent : public Event {
public:
	EVENT_CATEGORY(EventCategoryWindow);

protected:
	inline WindowEvent() {};
};

class WindowResizeEvent : public WindowEvent {
private:
	unsigned int width;
	unsigned int height;

public:
	EVENT_TYPE(WindowResize);

	inline WindowResizeEvent(unsigned int width, unsigned int height) : WindowEvent(), width(width), height(height) {};

	inline unsigned int getWidth() const {
		return width;
	}

	inline unsigned int getHeight() const {
		return height;
	}
};

class WindowCloseEvent : public WindowEvent {
public:
	EVENT_TYPE(WindowClose);

	inline WindowCloseEvent() : WindowEvent() {};
};

class WindowDropEvent : public WindowEvent {
private:
	std::string path;

public:
	EVENT_TYPE(WindowDrop);

	inline WindowDropEvent(const std::string& path) : WindowEvent(), path(path) {};

	inline std::string getPath() const {
		return path;
	}
};

class FrameBufferResizeEvent : public WindowEvent {
private:
	unsigned int width;
	unsigned int height;

public:
	EVENT_TYPE(FrameBufferResize);

	inline FrameBufferResizeEvent(unsigned int width, unsigned int height) : WindowEvent(), width(width), height(height) {};

	inline unsigned int getWidth() const {
		return width;
	}

	inline unsigned int getHeight() const {
		return height;
	}
};

};