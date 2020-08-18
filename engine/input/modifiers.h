#pragma once

namespace P3D::Engine {
	
namespace Modifier {
	extern const int NONE;
	extern const int SHIFT;
	extern const int CTRL;
	extern const int ALT;
	extern const int SUPER;
};

struct Modifiers {
private:
	int modifiers;

public:
	inline Modifiers(int modifiers) : modifiers(modifiers) {}

	int getModifiers() const;
	bool isCtrlPressed() const;
	bool isShiftPressed() const;
	bool isSuperPressed() const;
	bool isAltPressed() const;
};

};