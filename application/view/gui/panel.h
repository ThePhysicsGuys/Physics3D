#pragma once

#include "component.h"
#include "container.h"

#include "../engine/math/linalg/vec.h"

#include <vector>

class Panel : public Container {
public:
	Vec4 background;

	Panel(double x, double y);
	Panel(double x, double y, double width, double height);

	Vec2 resize() override;
	void render() override;
};