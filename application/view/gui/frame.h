#pragma once

#include <string>

#include "component.h"
#include "container.h"

#include "../engine/math/vec2.h"
#include "../engine/math/vec4.h"

class Button;
class Label;

class Frame : public Container {
public:
	Component* anchor;

	Button* closeButton;
	Button* minimizeButton;
	double buttonOffset;

	Label* title;

	double titleBarHeight;
	Vec4 titleBarColor;

	Vec4 backgroundColor;

	bool minimized = false;

	Frame();
	Frame(double x, double y, std::string title = "");
	Frame(double x, double y, double width, double height, std::string title = "");

	Vec2 resize() override;
	void render() override;

	Component* intersect(Vec2 point) override;

	void drag(Vec2 newPoint, Vec2 oldPoint) override;
	void disable() override;
	void enable() override;
};

