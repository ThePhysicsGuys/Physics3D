#pragma once

#include "component.h"

#include "../engine/math/vec2.h"
#include "../engine/math/vec4.h"

#include <vector>

class Panel : public Component {
public:
	double offset;
	double titleBarHeight;
	Vec4 titleBarColor;
	Vec4 backgroundColor;
	
	std::vector<Component*> children;

	Panel(double x, double y);
	Panel(double x, double y, double width, double height);

	void add(Component* component);
	void remove(Component* component);

	Vec2 resize() override;
	void render() override;
	void renderChildren();
};