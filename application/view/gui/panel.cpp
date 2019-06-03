#include "panel.h"

#include "gui.h"

#include "layout.h"
#include "../util/log.h"
#include "../engine/math/mathUtil.h"

Panel::Panel(double x, double y) : Container(x, y) {
	this->backgroundColor = GUI::panelBackgroundColor;
	this->padding = GUI::panelPadding;
	this->margin = GUI::panelMargin;
};

Panel::Panel(double x, double y, double width, double height) : Container(x, y, width, height) {
	this->backgroundColor = GUI::panelBackgroundColor;
	this->padding = GUI::panelPadding;
	this->margin = GUI::panelMargin;
};

Vec2 Panel::resize() {
	dimension = layout->resize(this);
	return dimension;
}

void Panel::render() {
	if (visible) {
		resize();

		// padding and margin to be added
		GUI::shader->update(backgroundColor);
		GUI::quad->resize(position, dimension);
		GUI::quad->render();

		renderChildren();
	}
}
