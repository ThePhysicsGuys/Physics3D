#include "core.h"

#include <GLFW/glfw3.h>

#include "modifiers.h"

namespace P3D::Engine {
	
int Modifiers::getModifiers() const {
	return modifiers;
}

bool Modifiers::isCtrlPressed() const {
	return modifiers & Modifier::CTRL;
}

bool Modifiers::isShiftPressed() const {
	return modifiers & Modifier::SHIFT;
}

bool Modifiers::isSuperPressed() const {
	return modifiers & Modifier::SUPER;
}

bool Modifiers::isAltPressed() const {
	return modifiers & Modifier::ALT;
}

namespace Modifier {
	const int NONE = 0;
	const int SHIFT = GLFW_MOD_SHIFT;
	const int CTRL = GLFW_MOD_CONTROL;
	const int ALT = GLFW_MOD_ALT;
	const int SUPER = GLFW_MOD_SUPER;
};

};