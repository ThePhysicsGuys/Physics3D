#pragma once

#include "component.h"

#include "../texture.h"

#include "../engine/math/vec2.h"

class ColorPicker;

typedef void (*ColorPickerAction) (ColorPicker*);

class ColorPicker : public Component {
private:
	Vec2 crosshair;

	bool colorPicking;
	bool brightnessPicking;
	bool alphaPicking;
public:
	ColorPickerAction action = [] (ColorPicker*) {};

	Vec4 hsva;

	Vec4 background;

	ColorPicker(double x, double y, double size);
	ColorPicker(double x, double y);

	void setRgba(Vec4 rgba);
	Vec4 getRgba();

	Vec2 resize() override;
	void render() override;

	void press(Vec2 point) override;
	void drag(Vec2 newPoint, Vec2 oldPoint) override;
	void release(Vec2 point) override;
};