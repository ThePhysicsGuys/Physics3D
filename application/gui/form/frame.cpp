#include "frame.h"

#include "gui.h"

#include "../util/log.h"
#include "../engine/math/mathUtil.h"

Frame::Frame(double x, double y) : Container(Vec2(x, y)) {
	this->backgroundColor = GUI::defaultPanelBackgroundColor;
	this->padding = GUI::defaultPanelPadding;
	this->margin = GUI::defaultPanelMargin;

	this->titleBarColor = GUI::defaultPanelTitleBarColor;
	this->titleBarHeight = GUI::defaultPanelTitleBarHeight;
	this->closeTexture = GUI::defaultPanelCloseTexture;
	this->closeButtonOffset = GUI::defaultPanelCloseButtonOffset;
};

Frame::Frame(double x, double y, double width, double height) : Container(Vec2(x, y), Vec2(width, height)) {
	this->backgroundColor = GUI::defaultPanelBackgroundColor;
	this->padding = GUI::defaultPanelPadding;
	this->margin = GUI::defaultPanelMargin;

	this->titleBarHeight = GUI::defaultPanelTitleBarHeight;
	this->titleBarColor = GUI::defaultPanelTitleBarColor;
	this->closeTexture = GUI::defaultPanelCloseTexture;
	this->closeButtonOffset = GUI::defaultPanelCloseButtonOffset;
};

Vec2 Frame::resize() {
	dimension = layout->resize(this);
	return dimension;
}

Component* Frame::intersect(Vec2 point) {
	for (auto iterator = children.begin(); iterator != children.end(); iterator++) {
		if (iterator->first->intersect(point))
			return iterator->first;
	}

	Vec2 titleBarDimension = Vec2(dimension.x + 2 * padding, titleBarHeight);
	Vec2 topleft = position + Vec2(-padding, padding + titleBarHeight);
	if (GUI::intersectsSquare(point, topleft, titleBarDimension)) {
		return this;
	}

	return nullptr;
}

void Frame::hover(Vec2 point) {
	// Closebutton
	Vec2 titleBarPosition = position + Vec2(-padding, padding + titleBarHeight);
	Vec2 titleBarDimension = Vec2(dimension.x + 2 * padding, titleBarHeight);
	Vec2 closeButtonPosition = titleBarPosition + Vec2(titleBarDimension.x - titleBarHeight + closeButtonOffset, -closeButtonOffset);
	Vec2 closeButtonDimension = Vec2(titleBarHeight - 2 * closeButtonOffset);

	if (GUI::intersectsSquare(point, closeButtonPosition, closeButtonDimension)) {
		closeButtonOffset = 0.007;
	} else {
		closeButtonOffset = GUI::defaultPanelCloseButtonOffset;
	}
}

void Frame::render() {
	if (visible) {
		resize();

		// TitleBar
		Vec2 titleBarPosition = position + Vec2(-padding, padding + titleBarHeight);
		Vec2 titleBarDimension = Vec2(dimension.x + 2 * padding, titleBarHeight);
		GUI::defaultShader->update(titleBarColor);
		GUI::defaultQuad->resize(titleBarPosition, titleBarDimension);
		GUI::defaultQuad->render();

		// Padding
		Vec2 offsetPosition = position + Vec2(-padding, padding);
		Vec2 offsetDimension = dimension + Vec2(padding) * 2;
		GUI::defaultShader->update(backgroundColor);
		GUI::defaultQuad->resize(offsetPosition, offsetDimension);
		GUI::defaultQuad->render();

		// Close button
		Vec2 closeButtonPosition = titleBarPosition + Vec2(titleBarDimension.x - titleBarHeight + closeButtonOffset, -closeButtonOffset);
		Vec2 closeButtonDimension = Vec2(titleBarHeight - 2 * closeButtonOffset);
		GUI::defaultShader->update(closeTexture);
		GUI::defaultQuad->resize(closeButtonPosition, closeButtonDimension);
		GUI::defaultQuad->render();

		// Content, no margin yet
		GUI::defaultShader->update(backgroundColor);
		GUI::defaultQuad->resize(position, dimension);
		GUI::defaultQuad->render();

		renderChildren();
	}
}
