#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "../engine/math/vec2.h"

class Component {
public:
	enum class Layout {
		ABSOLUTE = 0,
		RELATIVE = 1
	};

	Component* parent;

	Layout layout;
	bool resizing;

	Vec2 position;
	Vec2 dimension;

	Component(Vec2 position) : position(position), dimension(Vec2(0)), layout(Layout::RELATIVE), resizing(true) {};
	Component(Vec2 position, Vec2 dimension) : position(position), dimension(dimension), layout(Layout::RELATIVE), resizing(false) {};

	virtual Vec2 resize() = 0;
	virtual void render() = 0;

	Vec2 interpolate() {
		return Vec2(parent->position.x + parent->dimension.x * position.x, parent->position.y - parent->dimension.y * position.y);
	}
};