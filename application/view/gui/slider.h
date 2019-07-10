#pragma once

#include "component.h"

#include "../engine/math/vec2.h"
#include "../engine/math/vec4.h"

class Slider;

typedef void(*SliderAction) (Slider*);

class Slider : public Component {
public:
	SliderAction action = [] (Slider*) {};

	Vec4 handleColor;
	Vec4 backgroundColor;
	Vec4 foregroundFilledColor;
	Vec4 foregroundEmptyColor;

	double handleWidth;
	double barWidth;
	double barHeight;
	double handleHeight;

	double min = 0.0;
	double max = 1.0;
	double value = 0;

	Slider(double x, double y);
	Slider(double x, double y, double min, double max, double value);
	Slider(double x, double y, double width, double height);
	Slider(double x, double y, double width, double height, double min, double max, double value);

	void render() override;
	Vec2 resize() override;

	void drag(Vec2 newPoint, Vec2 oldPoint) override;
	void press(Vec2 point) override;
};