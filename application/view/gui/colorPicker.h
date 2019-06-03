#pragma once

#include "../texture.h"
#include "component.h"

#include "../engine/math/vec2.h"

class ColorPicker : public Component {
private:
	Vec2 crosshair;
public:
	Vec4 color;

	Vec4 background;

	ColorPicker(double x, double y, double size);
	ColorPicker(double x, double y);

	void setColor(Vec4 rgba);

	Vec2 resize() override;
	void render() override;

	void press(Vec2 point) override;
	void drag(Vec2 point) override;
};