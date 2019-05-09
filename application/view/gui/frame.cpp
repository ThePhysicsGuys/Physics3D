#include "frame.h"

#include "gui.h"

#include "../util/log.h"
#include "../engine/math/mathUtil.h"

Frame::Frame() : Frame(0, 0) {};

Frame::Frame(double x, double y, std::string name) : Container(x, y) {
	this->padding = GUI::defaultPadding;
	this->margin = GUI::defaultMargin;

	this->backgroundColor = GUI::defaultFrameBackgroundColor;
	this->titleBarColor = GUI::defaultFrameTitleBarColor;
	this->titleBarHeight = GUI::defaultFrameTitleBarHeight;

	this->buttonOffset = GUI::defaultFrameButtonOffset;

	this->closeButton = new Button(position.x, position.y, titleBarHeight - 2 * buttonOffset, titleBarHeight - 2 * buttonOffset, true);
	this->closeButton->parent = this;
	this->closeButton->idleTexture = GUI::defaultCloseButtonIdleTexture;
	this->closeButton->hoverTexture = GUI::defaultCloseButtonHoverTexture;
	this->closeButton->pressTexture = GUI::defaultCloseButtonPressTexture;
	this->closeButton->action = [] (Button* button) {
		button->parent->visible = false;
	};

	this->minimizeButton = new Button(position.x, position.y, titleBarHeight - 2 * buttonOffset, titleBarHeight - 2 * buttonOffset, true);
	this->minimizeButton->parent = this;
	this->minimizeButton->idleTexture = GUI:: defaultMinimizeButtonIdleTexture;
	this->minimizeButton->hoverTexture = GUI::defaultMinimizeButtonHoverTexture;
	this->minimizeButton->pressTexture = GUI::defaultMinimizeButtonPressTexture;
	this->minimizeButton->action = [] (Button* button) {
		Frame* frame = (Frame*) button->parent;
		frame->minimized = !frame->minimized;
	};

	title = new Label(name, position.x, position.y);
};

Frame::Frame(double x, double y, double width, double height, std::string name) : Container(x, y, width, height) {
	this->padding = GUI::defaultPadding;
	this->margin = GUI::defaultMargin;

	this->backgroundColor = GUI::defaultFrameBackgroundColor;
	this->titleBarColor = GUI::defaultFrameTitleBarColor;
	this->titleBarHeight = GUI::defaultFrameTitleBarHeight;

	this->buttonOffset = GUI::defaultFrameButtonOffset;

	this->closeButton = new Button(position.x, position.y, titleBarHeight - 2 * buttonOffset, titleBarHeight - 2 * buttonOffset, true);
	this->closeButton->parent = this;
	this->closeButton->idleTexture = GUI::defaultCloseButtonIdleTexture;
	this->closeButton->hoverTexture = GUI::defaultCloseButtonHoverTexture;
	this->closeButton->pressTexture = GUI::defaultCloseButtonPressTexture;
	this->closeButton->action = [] (Button* button) {
		button->parent->visible = false;
	};

	this->minimizeButton = new Button(position.x, position.y, titleBarHeight - 2 * buttonOffset, titleBarHeight - 2 * buttonOffset, true);
	this->minimizeButton->parent = this;
	this->minimizeButton->idleTexture = GUI::defaultMinimizeButtonIdleTexture;
	this->minimizeButton->hoverTexture = GUI::defaultMinimizeButtonHoverTexture;
	this->minimizeButton->pressTexture = GUI::defaultMinimizeButtonPressTexture;
	this->minimizeButton->action = [] (Button* button) {
		Frame* frame = (Frame*) button->parent;
		frame->minimized = !frame->minimized;
	};

	title = new Label(name, position.x, position.y);
};

Vec2 Frame::resize() {
	// Button
	closeButton->position = position + Vec2(width - titleBarHeight + buttonOffset, -buttonOffset);
	minimizeButton->position = position + Vec2(width - 2 * titleBarHeight + buttonOffset, -buttonOffset);

	// Title
	if (!title->text.empty()) {
		title->resize();
		double yOffset = (titleBarHeight - title->height) / 2;
		title->position = position + Vec2(yOffset, -yOffset);
	}

	// Content
	if (!minimized) {
		Vec2 positionOffset = Vec2(padding, -padding - titleBarHeight);
		Vec2 dimensionOffset = Vec2(2 * padding, 2 * padding + titleBarHeight);

		position += positionOffset;
		dimension -= dimensionOffset;

		if (title->text.empty())
			dimension = layout->resize(this);
		else
			dimension = layout->resize(this, Vec2(title->dimension.x + (titleBarHeight - title->height) / 2 + 2 * titleBarHeight, 0));


		position -= positionOffset;
		dimension += dimensionOffset;
	} else {
		dimension = Vec2(dimension.x, titleBarHeight);
	}

	return dimension;
}

Component* Frame::intersect(Vec2 point) {
	if (closeButton->intersect(point))
		return closeButton;

	if (minimizeButton->intersect(point))
		return minimizeButton;

	if (!minimized) {
		for (auto iterator = children.begin(); iterator != children.end(); iterator++) {
			if (iterator->first->intersect(point))
				return iterator->first;
		}
	}

	if (GUI::intersectsSquare(point, position, dimension)) {
		return this;
	}

	return nullptr;
}

void Frame::drag(Vec2 mouse) {
	position = mouse - GUI::intersectedPoint;
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

		// Buttons
		closeButton->render();
		minimizeButton->render();

		// Title
		if (!title->text.empty())
			title->render();

		if (!minimized) {
			// Padding
			Vec2 offsetPosition = titleBarPosition + Vec2(0, -titleBarHeight);
			Vec2 offsetDimension = dimension + Vec2(0, -titleBarHeight);
			GUI::defaultShader->update(backgroundColor);
			GUI::defaultQuad->resize(offsetPosition, offsetDimension);
			GUI::defaultQuad->render();

			// Content
			Vec2 contentPosition = position + Vec2(padding, -padding - titleBarHeight);
			Vec2 contentDimension = dimension - Vec2(2 * padding, 2 * padding + titleBarHeight);
			GUI::defaultShader->update(backgroundColor);
			GUI::defaultQuad->resize(contentPosition, contentDimension);
			GUI::defaultQuad->render();

			renderChildren();
		}

		// Outline
		Vec2 outlinePosition = titleBarPosition;
		Vec2 outlineDimension = dimension;
		GUI::defaultShader->update(GUI::COLOR::NAVY);
		GUI::defaultQuad->resize(outlinePosition, outlineDimension);
		GUI::defaultQuad->render(GL_LINE);
	}
}
