#include "mouse.h"

#include <stdexcept>
#include <map>

#include "GLFW\glfw3.h"

namespace Mouse {

	const Button UNKNOWN = { "none", -1 };
	const Button LEFT = { "mouse_left", GLFW_MOUSE_BUTTON_LEFT };
	const Button RIGHT = {"mouse_right", GLFW_MOUSE_BUTTON_RIGHT };
	const Button MIDDLE = { "mouse_middle", GLFW_MOUSE_BUTTON_MIDDLE };
	const Button MOUSE_4 = { "mouse_4", GLFW_MOUSE_BUTTON_4 };
	const Button MOUSE_5 = { "mouse_5", GLFW_MOUSE_BUTTON_5 };
	const Button MOUSE_6 = { "mouse_6", GLFW_MOUSE_BUTTON_6 };
	const Button MOUSE_7 = { "mouse_7", GLFW_MOUSE_BUTTON_7 };
	const Button MOUSE_8 = { "mouse_8", GLFW_MOUSE_BUTTON_8 };

	const int MOUSE_FIRST = GLFW_MOUSE_BUTTON_LAST;
	const int MOUSE_LAST = GLFW_MOUSE_BUTTON_LEFT;

	const int PRESS = GLFW_PRESS;
	const int RELEASE = GLFW_RELEASE;
	const int REPEAT = GLFW_REPEAT;

	std::map<std::string, Button> buttonmap = {
		{ UNKNOWN.name, UNKNOWN },
		{ LEFT.name, LEFT },
		{ RIGHT.name, RIGHT },
		{ MIDDLE.name, MIDDLE },
		{ MOUSE_4.name, MOUSE_4 },
		{ MOUSE_5.name, MOUSE_5 },
		{ MOUSE_6.name, MOUSE_6 },
		{ MOUSE_7.name, MOUSE_7 },
		{ MOUSE_8.name, MOUSE_8 }
	};

	Button getKey(std::string name) {
		try {
			Button& key = buttonmap.at(name);
			return key;
		} catch (std::out_of_range& const e) {
			return UNKNOWN;
		}
	}

	Button getKey(int code) {
		for (auto button : buttonmap) {
			if (button.second.code == code)
				return button.second;
		}

		return UNKNOWN;
	}
}