#pragma once

namespace Mouse {

	struct Button {
		std::string name;
		int code;

		// Button <-> Button
		bool operator==(const Button& other) const {
			return other.code == code;
		}

		// Key <-> int
		bool operator==(int other) const {
			return other == code;
		}

		// Key <-> string
		bool operator==(const std::string& other) const {
			return other == name;
		}
	};
							 
	extern const Button LEFT;
	extern const Button RIGHT;
	extern const Button MIDDLE;
	extern const Button MOUSE_4;
	extern const Button MOUSE_5;
	extern const Button MOUSE_6;
	extern const Button MOUSE_7;
	extern const Button MOUSE_8;

	extern const int MOUSE_FIRST;
	extern const int MOUSE_LAST;
	
	extern const int PRESS;
	extern const int RELEASE;
	extern const int REPEAT;

	Button getKey(std::string name);
	Button getKey(int code);


};