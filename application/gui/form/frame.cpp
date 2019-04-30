#include "frame.h"

#include "gui.h"

#include "../util/log.h"
#include "../engine/math/mathUtil.h"

Frame::Frame() : Frame(0, 0) {};

Frame::Frame(double x, double y) : Container(x, y) {
	this->padding = GUI::defaultFramePadding;
	this->margin = GUI::defaultFrameMargin;

	this->backgroundColor = GUI::defaultFrameBackgroundColor;
	this->titleBarColor = GUI::defaultFrameTitleBarColor;
	this->titleBarHeight = GUI::defaultFrameTitleBarHeight;

	this->closeButtonOffset = GUI::defaultFrameCloseButtonOffset;
	this->closeButton = new Button(position.x, position.y, titleBarHeight - 2 * closeButtonOffset, titleBarHeight - 2 * closeButtonOffset, true);
	this->closeButton->parent = this;
	this->closeButton->idleTexture = GUI::defaultButtonIdleTexture;
	this->closeButton->hoverTexture = GUI::defaultButtonHoverTexture;
	this->closeButton->pressTexture = GUI::defaultButtonPressTexture;

	this->closeButton->action = [] (Button* button) {
		button->parent->visible = false;
	};
};

Frame::Frame(double x, double y, double width, double height) : Container(x, y, width, height) {
	this->padding = GUI::defaultFramePadding;
	this->margin = GUI::defaultFrameMargin;

	this->backgroundColor = GUI::defaultFrameBackgroundColor;
	this->titleBarHeight = GUI::defaultFrameTitleBarHeight;
	this->titleBarColor = GUI::defaultFrameTitleBarColor;

	this->closeButtonOffset = GUI::defaultFrameCloseButtonOffset;
	this->closeButton = new Button(0, 0, titleBarHeight - 2 * closeButtonOffset, titleBarHeight - 2 * closeButtonOffset, true);
	this->closeButton->parent = this;
	this->closeButton->idleTexture = GUI::defaultButtonIdleTexture;
	this->closeButton->hoverTexture = GUI::defaultButtonHoverTexture;
	this->closeButton->pressTexture = GUI::defaultButtonPressTexture;

	this->closeButton->action = [] (Button* button) {
		button->parent->visible = false;
	};
};

Vec2 Frame::resize() {
	// Button
	closeButton->position = position + Vec2(width - titleBarHeight + closeButtonOffset, -closeButtonOffset);

	// Content
	Vec2 positionOffset = Vec2(padding, -padding - titleBarHeight);
	Vec2 dimensionOffset = Vec2(2 * padding, 2 * padding + titleBarHeight);
	
	position += positionOffset;
	dimension -= dimensionOffset;
	
	dimension = layout->resize(this);
	
	position -= positionOffset;
	dimension += dimensionOffset;
	
	return dimension;
}

Component* Frame::intersect(Vec2 point) {
	if (closeButton->intersect(point))
		return closeButton;

	for (auto iterator = children.begin(); iterator != children.end(); iterator++) {
		if (iterator->first->intersect(point))
			return iterator->first;
	}

	if (GUI::intersectsSquare(point, position, dimension)) {
		return this;
	}

	return nullptr;
}

void Frame::hover(Vec2 point) {

}

void Frame::enter() {
	Log::debug("Enter frame");
}

void Frame::exit() {
	Log::debug("Exit frame");
}

void Frame::render() {
	if (visible) {
		resize();

		// TitleBar
		Vec2 titleBarPosition = position;
		Vec2 titleBarDimension = Vec2(width, titleBarHeight);
		GUI::defaultShader->update(titleBarColor);
		GUI::defaultQuad->resize(titleBarPosition, titleBarDimension);
		GUI::defaultQuad->render();

		// Padding
		Vec2 offsetPosition = titleBarPosition + Vec2(0, -titleBarHeight);
		Vec2 offsetDimension = dimension + Vec2(0, -titleBarHeight);
		GUI::defaultShader->update(backgroundColor);
		GUI::defaultQuad->resize(offsetPosition, offsetDimension);
		GUI::defaultQuad->render();

		// Close button
		closeButton->render();

		// Content
		Vec2 contentPosition = position + Vec2(padding, -padding - titleBarHeight);
		Vec2 contentDimension = dimension - Vec2(2 * padding, 2 * padding + titleBarHeight);
		GUI::defaultShader->update(backgroundColor);
		GUI::defaultQuad->resize(contentPosition, contentDimension);
		GUI::defaultQuad->render();

		// Outline
		Vec2 outlinePosition = titleBarPosition;
		Vec2 outlineDimension = dimension;
		GUI::defaultShader->update(GUI::COLOR::NAVY);
		GUI::defaultQuad->resize(outlinePosition, outlineDimension);
		GUI::defaultQuad->render(GL_LINE);

		renderChildren();
	}
}
