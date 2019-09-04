#pragma once

#include "event.h"

class KeyEvent : public Event {
private:
	int key;
	int modifiers;

public:
	EVENT_CATEGORY(EventCategoryInput | EventCategoryKeyboard);

	inline int getKey() const {
		return key;
	}

	inline int getModifiers() const {
		return modifiers;
	}

protected:
	KeyEvent(int key, int modifiers) : key(key), modifiers(modifiers) {}
};

class KeyPressEvent : public KeyEvent {
public:
	EVENT_TYPE(KeyPress);

	KeyPressEvent(int key, int modifiers = 0) : KeyEvent(key, modifiers) {}
};

class KeyReleaseEvent : public KeyEvent {
public:
	EVENT_TYPE(KeyRelease);

	KeyReleaseEvent(int key, int modifiers = 0) : KeyEvent(key, modifiers) {}
};