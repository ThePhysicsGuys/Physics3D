#include "core.h"

#include "mouse.h"

#include <stdexcept>

#include <GLFW/glfw3.h>

namespace Engine {

std::string Button::getName() const {
	return name;
}

int Button::getCode() const {
	return code;
}

// Button <-> Button
bool Button::operator==(const Button& other) const {
	return other.code == code;
}

bool Button::operator!=(const Button& other) const {
	return other.code != code;
}

// Key <-> int
bool Button::operator==(int other) const {
	return other == code;
}

bool Button::operator!=(int other) const {
	return other != code;
}

// Key <-> string
bool Button::operator==(const std::string& other) const {
	return other == name;
}

bool Button::operator!=(const std::string& other) const {
	return other != name;
}

namespace Mouse {

	const Button UNKNOWN = { "none"        , -1                       };
	const Button LEFT    = { "mouse_left"  , GLFW_MOUSE_BUTTON_LEFT   };
	const Button RIGHT   = {"mouse_right"  , GLFW_MOUSE_BUTTON_RIGHT  };
	const Button MIDDLE  = { "mouse_middle", GLFW_MOUSE_BUTTON_MIDDLE };
	const Button MOUSE_4 = { "mouse_4"     , GLFW_MOUSE_BUTTON_4      };
	const Button MOUSE_5 = { "mouse_5"     , GLFW_MOUSE_BUTTON_5      };
	const Button MOUSE_6 = { "mouse_6"     , GLFW_MOUSE_BUTTON_6      };
	const Button MOUSE_7 = { "mouse_7"     , GLFW_MOUSE_BUTTON_7      };
	const Button MOUSE_8 = { "mouse_8"     , GLFW_MOUSE_BUTTON_8      };

	const int MOUSE_FIRST = GLFW_MOUSE_BUTTON_LAST;
	const int MOUSE_LAST = GLFW_MOUSE_BUTTON_LEFT;

	const int PRESS = GLFW_PRESS;
	const int RELEASE = GLFW_RELEASE;
	const int REPEAT = GLFW_REPEAT;

	std::map<int, Button> buttonmap = {
		{ UNKNOWN.getCode(), UNKNOWN },
		{ LEFT.getCode()   , LEFT    },
		{ RIGHT.getCode()  , RIGHT   },
		{ MIDDLE.getCode() , MIDDLE  },
		{ MOUSE_4.getCode(), MOUSE_4 },
		{ MOUSE_5.getCode(), MOUSE_5 },
		{ MOUSE_6.getCode(), MOUSE_6 },
		{ MOUSE_7.getCode(), MOUSE_7 },
		{ MOUSE_8.getCode(), MOUSE_8 }
	};

	Button getButton(std::string name) {
		for (auto& button : buttonmap) {
			if (button.second.getName() == name)
				return button.second;
		}
	}

	Button getButton(int code) {
		try {
			Button& key = buttonmap.at(code);
			return key;
		} catch (const std::out_of_range&) {
			return UNKNOWN;
		}
	}
}

};