#pragma once

#include "component.h"

class ColorPicker;
class Texture;

typedef void (*ColorPickerAction) (ColorPicker*);

class ColorPicker : public Component {
private:
	Vec2 crosshair;

	bool colorPicking;
	bool brightnessPicking;
	bool alphaPicking;

public:
	ColorPickerAction action = [] (ColorPicker*) {};

	Color hsva;
	Color background;

	Component* focus = nullptr;

	ColorPicker(double x, double y, double size);
	ColorPicker(double x, double y);

	void setRgba(Color rgba);
	Color getRgba();

	Vec2 resize() override;
	void render() override;

	void press(Vec2 point) override;
	void drag(Vec2 newPoint, Vec2 oldPoint) override;
	void release(Vec2 point) override;
};