#pragma once

namespace P3D::Engine {

struct Button {
private:
	std::string name;
	int code;

public:
	inline Button(const std::string& name, int code) : name(name), code(code) {}

	std::string getName() const;
	int getCode() const;

	// Button <-> Button
	bool operator==(const Button& other) const;
	bool operator!=(const Button& other) const;

	// Key <-> int
	bool operator==(int other) const;
	bool operator!=(int other) const;

	// Key <-> string
	bool operator==(const std::string& other) const;
	bool operator!=(const std::string& other) const;
};
							 
namespace Mouse {

	Button getButton(std::string name);
	Button getButton(int code);

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

};

};