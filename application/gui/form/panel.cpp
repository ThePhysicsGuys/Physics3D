#include "panel.h"

#include "gui.h"

#include "layout.h"
#include "../util/log.h"
#include "../engine/math/mathUtil.h"

Panel::Panel(double x, double y) : Container(x, y) {
	this->backgroundColor = GUI::defaultPanelBackgroundColor;
	this->padding = GUI::defaultPanelPadding;
	this->margin = GUI::defaultPanelMargin;
};

Panel::Panel(double x, double y, double width, double height) : Container(x, y, width, height) {
	this->backgroundColor = GUI::defaultPanelBackgroundColor;
	this->padding = GUI::defaultPanelPadding;
	this->margin = GUI::defaultPanelMargin;
};

Vec2 Panel::resize() {
	dimension = layout->resize(this);
	return dimension;
}

void Panel::render() {
	if (visible) {
		resize();

		// padding and margin to be added
		GUI::defaultShader->update(backgroundColor);
		GUI::defaultQuad->resize(position, dimension);
		GUI::defaultQuad->render();

		renderChildren();
	}
}
