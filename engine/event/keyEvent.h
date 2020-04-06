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
	inline KeyEvent(int key, int modifiers) : key(key), modifiers(modifiers) {}
};

class DoubleKeyPressEvent : public KeyEvent {
public:
	EVENT_TYPE(KeyDoublePress);

	inline DoubleKeyPressEvent(int key, int modifiers) : KeyEvent(key, modifiers) {}
};

class KeyPressEvent : public KeyEvent {
private:
	bool repeat;

public:
	EVENT_TYPE(KeyPress);

	inline KeyPressEvent(int key, int modifiers = 0, bool repeat = false) : KeyEvent(key, modifiers), repeat(repeat) {}

	inline bool isRepeated() const {
		return repeat;
	}
};

class KeyReleaseEvent : public KeyEvent {
public:
	EVENT_TYPE(KeyRelease);

	inline KeyReleaseEvent(int key, int modifiers = 0) : KeyEvent(key, modifiers) {}
};