#pragma once

namespace Engine {

struct Key {
private:
	std::string name;
	int code;

public:
	inline Key(const std::string& name, int code) : name(name), code(code) {}

	std::string getName() const;
	int getCode() const;

	// Key <-> Key
	bool operator==(const Key& other) const;
	bool operator!=(const Key& other) const;
	bool operator>=(const Key& other) const;
	bool operator>(const Key& other) const;
	bool operator<=(const Key& other) const;
	bool operator<(const Key& other) const;

	// Key <-> int
	bool operator==(int other) const;
	bool operator!=(int other) const;
	bool operator>=(int other) const;
	bool operator>(int other) const;
	bool operator<=(int other) const;
	bool operator<(int other) const;

	// Key <-> string
	bool operator==(const std::string& other) const;
	bool operator!=(const std::string& other) const;
};

namespace Keyboard {

	Key getKey(const std::string& name);
	Key getKey(int code);

	extern const Key KEY_UNKNOWN;
	extern const Key KEY_SPACE;
	extern const Key KEY_APOSTROPHE;
	extern const Key KEY_COMMA;
	extern const Key KEY_MINUS;
	extern const Key KEY_PERIOD;
	extern const Key KEY_SLASH;
	extern const Key KEY_NUMBER_0;
	extern const Key KEY_NUMBER_1;
	extern const Key KEY_NUMBER_2;
	extern const Key KEY_NUMBER_3;
	extern const Key KEY_NUMBER_4;
	extern const Key KEY_NUMBER_5;
	extern const Key KEY_NUMBER_6;
	extern const Key KEY_NUMBER_7;
	extern const Key KEY_NUMBER_8;
	extern const Key KEY_NUMBER_9;
	extern const Key KEY_SEMICOLON;
	extern const Key KEY_EQUAL;
	extern const Key KEY_A;
	extern const Key KEY_B;
	extern const Key KEY_C;
	extern const Key KEY_D;
	extern const Key KEY_E;
	extern const Key KEY_F;
	extern const Key KEY_G;
	extern const Key KEY_H;
	extern const Key KEY_I;
	extern const Key KEY_J;
	extern const Key KEY_K;
	extern const Key KEY_L;
	extern const Key KEY_M;
	extern const Key KEY_N;
	extern const Key KEY_O;
	extern const Key KEY_P;
	extern const Key KEY_Q;
	extern const Key KEY_R;
	extern const Key KEY_S;
	extern const Key KEY_T;
	extern const Key KEY_U;
	extern const Key KEY_V;
	extern const Key KEY_W;
	extern const Key KEY_X;
	extern const Key KEY_Y;
	extern const Key KEY_Z;
	extern const Key KEY_LEFT_BRACKET;
	extern const Key KEY_BACKSLASH;
	extern const Key KEY_RIGHT_BRACKET;
	extern const Key KEY_GRAVE_ACCENT;
	extern const Key KEY_WORLD_1;
	extern const Key KEY_WORLD_2;
	extern const Key KEY_ESCAPE;
	extern const Key KEY_ENTER;
	extern const Key KEY_TAB;
	extern const Key KEY_BACKSPACE;
	extern const Key KEY_INSERT;
	extern const Key KEY_DELETE;
	extern const Key KEY_RIGHT;
	extern const Key KEY_LEFT;
	extern const Key KEY_DOWN;
	extern const Key KEY_UP;
	extern const Key KEY_PAGE_UP;
	extern const Key KEY_PAGE_DOWN;
	extern const Key KEY_HOME;
	extern const Key KEY_END;
	extern const Key KEY_CAPS_LOCK;
	extern const Key KEY_SCROLL_LOCK;
	extern const Key KEY_NUM_LOCK;
	extern const Key KEY_PRINT_SCREEN;
	extern const Key KEY_PAUSE;
	extern const Key KEY_F1;
	extern const Key KEY_F2;
	extern const Key KEY_F3;
	extern const Key KEY_F4;
	extern const Key KEY_F5;
	extern const Key KEY_F6;
	extern const Key KEY_F7;
	extern const Key KEY_F8;
	extern const Key KEY_F9;
	extern const Key KEY_F10;
	extern const Key KEY_F11;
	extern const Key KEY_F12;
	extern const Key KEY_F13;
	extern const Key KEY_F14;
	extern const Key KEY_F15;
	extern const Key KEY_F16;
	extern const Key KEY_F17;
	extern const Key KEY_F18;
	extern const Key KEY_F19;
	extern const Key KEY_F20;
	extern const Key KEY_F21;
	extern const Key KEY_F22;
	extern const Key KEY_F23;
	extern const Key KEY_F24;
	extern const Key KEY_F25;
	extern const Key KEY_NUMPAD_0;
	extern const Key KEY_NUMPAD_1;
	extern const Key KEY_NUMPAD_2;
	extern const Key KEY_NUMPAD_3;
	extern const Key KEY_NUMPAD_4;
	extern const Key KEY_NUMPAD_5;
	extern const Key KEY_NUMPAD_6;
	extern const Key KEY_NUMPAD_7;
	extern const Key KEY_NUMPAD_8;
	extern const Key KEY_NUMPAD_9;
	extern const Key KEY_NUMPAD_DECIMAL;
	extern const Key KEY_NUMPAD_DIVIDE;
	extern const Key KEY_NUMPAD_MULTIPLY;
	extern const Key KEY_NUMPAD_SUBTRACT;
	extern const Key KEY_NUMPAD_ADD;
	extern const Key KEY_NUMPAD_ENTER;
	extern const Key KEY_NUMPAD_EQUAL;
	extern const Key KEY_LEFT_SHIFT;
	extern const Key KEY_LEFT_CONTROL;
	extern const Key KEY_LEFT_ALT;
	extern const Key KEY_LEFT_SUPER;
	extern const Key KEY_RIGHT_SHIFT;
	extern const Key KEY_RIGHT_CONTROL;
	extern const Key KEY_RIGHT_ALT;
	extern const Key KEY_RIGHT_SUPER;
	extern const Key KEY_MENU;

	extern const int KEY_FIRST;
	extern const int KEY_LAST;
	extern const int KEY_PRESS;
	extern const int KEY_RELEASE;
	extern const int KEY_REPEAT;

}

};