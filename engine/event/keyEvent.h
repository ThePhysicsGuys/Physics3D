#pragma once

#include "event.h"
#include "../input/keyboard.h"
#include "../input/modifiers.h"

namespace Engine {

class KeyEvent : public Event {
private:
	Key key;
	Modifiers modifiers;

public:
	EVENT_CATEGORY(EventCategoryInput | EventCategoryKeyboard);

	inline Key getKey() const {
		return key;
	}

	inline Modifiers getModifiers() const {
		return modifiers;
	}

protected:
	inline KeyEvent(const Key& key, const Modifiers& modifiers) : key(key), modifiers(modifiers) {}
};

class DoubleKeyPressEvent : public KeyEvent {
public:
	EVENT_TYPE(KeyDoublePress);

	inline DoubleKeyPressEvent(const Key& key, const Modifiers& modifiers) : KeyEvent(key, modifiers) {}
};

class KeyPressEvent : public KeyEvent {
private:
	bool repeat;

public:
	EVENT_TYPE(KeyPress);

	inline KeyPressEvent(const Key& key, const Modifiers& modifiers = 0, bool repeat = false) : KeyEvent(key, modifiers), repeat(repeat) {}

	inline bool isRepeated() const {
		return repeat;
	}
};

class KeyReleaseEvent : public KeyEvent {
public:
	EVENT_TYPE(KeyRelease);

	inline KeyReleaseEvent(const Key& key, const Modifiers& modifiers = 0) : KeyEvent(key, modifiers) {}
};

};