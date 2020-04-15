#pragma once

#include "event.h"
#include "../input/keyboard.h"

class KeyEvent : public Event {
private:
	Keyboard::Key key;
	Keyboard::Modifiers modifiers;

public:
	EVENT_CATEGORY(EventCategoryInput | EventCategoryKeyboard);

	inline Keyboard::Key getKey() const {
		return key;
	}

	inline Keyboard::Modifiers getModifiers() const {
		return modifiers;
	}

protected:
	inline KeyEvent(const Keyboard::Key& key, const Keyboard::Modifiers& modifiers) : key(key), modifiers(modifiers) {}
};

class DoubleKeyPressEvent : public KeyEvent {
public:
	EVENT_TYPE(KeyDoublePress);

	inline DoubleKeyPressEvent(const Keyboard::Key& key, const Keyboard::Modifiers& modifiers) : KeyEvent(key, modifiers) {}
};

class KeyPressEvent : public KeyEvent {
private:
	bool repeat;

public:
	EVENT_TYPE(KeyPress);

	inline KeyPressEvent(const Keyboard::Key& key, const Keyboard::Modifiers& modifiers = 0, bool repeat = false) : KeyEvent(key, modifiers), repeat(repeat) {}

	inline bool isRepeated() const {
		return repeat;
	}
};

class KeyReleaseEvent : public KeyEvent {
public:
	EVENT_TYPE(KeyRelease);

	inline KeyReleaseEvent(const Keyboard::Key& key, const Keyboard::Modifiers& modifiers = 0) : KeyEvent(key, modifiers) {}
};