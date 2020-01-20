#include "core.h"

#include "panel.h"

#include "gui.h"
#include "layout.h"
#include "path/path.h"
#include "mesh/primitive.h"
#include "../physics/math/mathUtil.h"

Panel::Panel(double x, double y) : Container(x, y) {
	this->background = COLOR::BACK;
	this->padding = GUI::padding;
	this->margin = GUI::margin;
};

Panel::Panel(double x, double y, double width, double height) : Container(x, y, width, height) {
	this->background = COLOR::BACK;
	this->padding = GUI::padding;
	this->margin = GUI::margin;
};

Vec2 Panel::resize() {
	Vec2 positionOffset = Vec2(padding, -padding);
	Vec2 dimensionOffset = Vec2(2 * padding);

	position += positionOffset;
	dimension -= dimensionOffset;

	dimension = layout->resize(this);

	position -= positionOffset;
	dimension += dimensionOffset;

	return dimension;
}

void Panel::render() {
	if (visible) {

		Color blendColor = (disabled) ? COLOR::DISABLED : COLOR::WHITE;

		resize();

		Path::rectFilled(position, dimension, 0, COLOR::blend(background, blendColor));

		renderChildren();
	}
}
