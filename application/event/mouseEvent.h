#pragma once

#include "event.h"

class MouseEvent : public Event {
public:
	EVENT_CATEGORY(EventCategoryMouse | EventCategoryInput);

protected:
	MouseEvent() {};
};

class MouseMoveEvent : public MouseEvent {
private:
	int x;
	int y;
public:
	EVENT_TYPE(MouseMove);

	MouseMoveEvent(int x, int y) : x(x), y(y) {}

	inline int getX() const {
		return x;
	}

	inline int getY() const {
		return y;
	}
};

class MouseScrollEvent : public MouseEvent {
private:
	int xOffset;
	int yOffset;
	
public:
	EVENT_TYPE(MouseScroll);

	MouseScrollEvent(int xOffset, int yOffset) : xOffset(xOffset), yOffset(yOffset) {}

	inline int getXOffset() const {
		return xOffset;
	}

	inline int getYOffset() const {
		return yOffset;
	}
};

class MouseEnterEvent : public MouseEvent {
public:
	EVENT_TYPE(MouseEnter);

	MouseEnterEvent() {}
};

class MouseExitEvent : public MouseEvent {
public:
	EVENT_TYPE(MouseExit);

	MouseExitEvent() {}
};

class MouseButtonEvent : public MouseEvent {
private:
	int button;
	int modifiers;

public:
	EVENT_CATEGORY(EventCategoryMouse | EventCategoryInput);

	inline int getButton() const {
		return button;
	}

	inline int getModifiers() const {
		return modifiers;
	}

protected:
	MouseButtonEvent(int button, int modifiers) : button(button), modifiers(modifiers) {}
};

class MousePressEvent : public MouseButtonEvent {
public:
	EVENT_TYPE(MousePress);

	MousePressEvent(int button, int modifiers = 0) : MouseButtonEvent(button, modifiers) {}
};

class MouseReleaseEvent : public MouseButtonEvent {
public:
	EVENT_TYPE(MouseRelease);

	MouseReleaseEvent(int button, int modifiers = 0) : MouseButtonEvent(button, modifiers) {}
};