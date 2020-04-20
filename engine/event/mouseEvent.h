#pragma once

#include "event.h"
#include "../input/mouse.h"
#include "../input/modifiers.h"

namespace Engine {

class MouseEvent : public Event {
private:
	int x;
	int y;

protected:
	inline MouseEvent(int x, int y) : x(x), y(y) {};

public:
	EVENT_CATEGORY(EventCategoryMouse | EventCategoryInput);

	// Returns the x position at the time of the event
	inline int getX() const {
		return x;
	}

	// Returns the y position at the time of the event
	inline int getY() const {
		return y;
	}
};

class MouseMoveEvent : public MouseEvent {
private:
	int newX;
	int newY;

public:
	EVENT_TYPE(MouseMove);

	inline MouseMoveEvent(int oldX, int oldY, int newX, int newY) : MouseEvent(oldX, oldY), newX(newX), newY(newY) {}

	inline int getOldX() const {
		return getX();
	}

	inline int getOldY() const {
		return getY();
	}

	inline int getNewX() const {
		return newX;
	}

	inline int getNewY() const {
		return newY;
	}
};

class MouseScrollEvent : public MouseEvent {
private:
	int xOffset;
	int yOffset;

public:
	EVENT_TYPE(MouseScroll);

	inline MouseScrollEvent(int x, int y, int xOffset, int yOffset) : MouseEvent(x, y), xOffset(xOffset), yOffset(yOffset) {}

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

	inline MouseEnterEvent(int x, int y) : MouseEvent(x, y) {}
};

class MouseExitEvent : public MouseEvent {
public:
	EVENT_TYPE(MouseExit);

	inline MouseExitEvent(int x, int y) : MouseEvent(x, y) {}
};

class MouseButtonEvent : public MouseEvent {
private:
	Button button;
	Modifiers modifiers;

protected:
	inline MouseButtonEvent(int x, int y, const Button& button, const Modifiers& modifiers = Modifier::NONE) : MouseEvent(x, y), button(button), modifiers(modifiers) {}

public:
	EVENT_CATEGORY(EventCategoryMouse | EventCategoryMouseButton | EventCategoryInput);

	inline Button getButton() const {
		return button;
	}

	inline Modifiers getModifiers() const {
		return modifiers;
	}
};

class MousePressEvent : public MouseButtonEvent {
public:
	EVENT_TYPE(MousePress);

	inline MousePressEvent(int x, int y, const Button& button, const Modifiers& modifiers = Modifier::NONE) : MouseButtonEvent(x, y, button, modifiers) {}
};

class MouseReleaseEvent : public MouseButtonEvent {
public:
	EVENT_TYPE(MouseRelease);

	inline MouseReleaseEvent(int x, int y, const Button& button, const Modifiers& modifiers = Modifier::NONE) : MouseButtonEvent(x, y, button, modifiers) {}
};

class MouseDragEvent : public MouseButtonEvent {
private:
	int newX;
	int newY;

public:
	EVENT_TYPE(MouseDrag);

	inline MouseDragEvent(int oldX, int oldY, int newX, int newY, const Button& button, const Modifiers& modifiers) : MouseButtonEvent(oldX, oldY, button, modifiers), newX(newX), newY(newY) {}

	inline int getOldX() const {
		return getX();
	}

	inline int getOldY() const {
		return getY();
	}

	inline int getNewX() const {
		return newX;
	}

	inline int getNewY() const {
		return newY;
	}

	inline bool isLeftDragging() const {
		return getButton() == Mouse::LEFT;
	}

	inline bool isMiddleDragging() const {
		return getButton() == Mouse::MIDDLE;
	}

	inline bool isRightDragging() const {
		return getButton() == Mouse::RIGHT;
	}
};

};