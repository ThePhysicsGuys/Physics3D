#pragma once

#include "component.h"
#include "container.h"

class Panel : public Container {
public:
	Color background;

	Panel(double x, double y);
	Panel(double x, double y, double width, double height);

	Vec2 resize() override;
	void render() override;
};