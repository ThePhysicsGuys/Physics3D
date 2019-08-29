#include "frame.h"

#include "path.h"
#include "button.h"
#include "label.h"

#include "../screen.h"
#include "../texture.h"
#include "../shaderProgram.h"
#include "../renderUtils.h"

#include "../mesh/primitive.h"

#include "../buffers/frameBuffer.h"

#include "../util/log.h"
#include "../engine/math/mathUtil.h"

Frame::Frame() : Frame(0, 0) {

};

Frame::Frame(double x, double y, std::string name) : Container(x, y) {
	this->backgroundColor = GUI::frameBackgroundColor;
	this->titleBarColor = GUI::frameTitleBarColor;
	this->titleBarHeight = GUI::frameTitleBarHeight;

	this->buttonOffset = GUI::frameButtonOffset;

	this->closeButton = new Button(position.x, position.y, titleBarHeight - 2 * buttonOffset, titleBarHeight - 2 * buttonOffset, true);
	this->closeButton->parent = this;
	this->closeButton->idleTexture = GUI::closeButtonIdleTexture;
	this->closeButton->hoverTexture = GUI::closeButtonHoverTexture;
	this->closeButton->pressTexture = GUI::closeButtonPressTexture;
	this->closeButton->action = [] (Button* button) {
		button->parent->visible = false;
	};

	this->minimizeButton = new Button(position.x, position.y, titleBarHeight - 2 * buttonOffset, titleBarHeight - 2 * buttonOffset, true);
	this->minimizeButton->parent = this;
	this->minimizeButton->idleTexture = GUI:: minimizeButtonIdleTexture;
	this->minimizeButton->hoverTexture = GUI::minimizeButtonHoverTexture;
	this->minimizeButton->pressTexture = GUI::minimizeButtonPressTexture;
	this->minimizeButton->action = [] (Button* button) {
		Frame* frame = static_cast<Frame*>(button->parent);
		frame->minimized = !frame->minimized;
	};

	title = new Label(name, position.x, position.y);

	anchor = nullptr;
};

Frame::Frame(double x, double y, double width, double height, std::string name) : Container(x, y, width, height) {
	this->padding = GUI::padding;
	this->margin = GUI::margin;

	this->backgroundColor = GUI::frameBackgroundColor;
	this->titleBarColor = GUI::frameTitleBarColor;
	this->titleBarHeight = GUI::frameTitleBarHeight;

	this->buttonOffset = GUI::frameButtonOffset;

	this->closeButton = new Button(position.x, position.y, titleBarHeight - 2 * buttonOffset, titleBarHeight - 2 * buttonOffset, true);
	this->closeButton->parent = this;
	this->closeButton->idleTexture = GUI::closeButtonIdleTexture;
	this->closeButton->hoverTexture = GUI::closeButtonHoverTexture;
	this->closeButton->pressTexture = GUI::closeButtonPressTexture;
	this->closeButton->action = [] (Button* button) {
		button->parent->visible = false;
	};

	this->minimizeButton = new Button(position.x, position.y, titleBarHeight - 2 * buttonOffset, titleBarHeight - 2 * buttonOffset, true);
	this->minimizeButton->parent = this;
	this->minimizeButton->idleTexture = GUI::minimizeButtonIdleTexture;
	this->minimizeButton->hoverTexture = GUI::minimizeButtonHoverTexture;
	this->minimizeButton->pressTexture = GUI::minimizeButtonPressTexture;
	this->minimizeButton->action = [] (Button* button) {
		Frame* frame = (Frame*) button->parent;
		frame->minimized = !frame->minimized;
	};

	title = new Label(name, position.x, position.y);

	anchor = nullptr;
};

Vec2 Frame::resize() {
	if (anchor)
		position = Vec2(anchor->position.x + anchor->dimension.x + 2 * GUI::padding, anchor->position.y);

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
		for (auto component : children) {
			if (component.first->intersect(point))
				return component.first;
		}
	}

	if (GUI::intersectsSquare(point, position, dimension)) {
		return this;
	}

	return nullptr;
}

void Frame::disable() {
	disabled = true;

	minimizeButton->disable();
	closeButton->disable();
	title->disable();

	for (auto component : children)
		component.first->disable();
}

void Frame::enable() {
	disabled = false;

	minimizeButton->enable();
	closeButton->enable();
	title->enable();

	for (auto component : children)
		component.first->enable();
}

void Frame::drag(Vec2 newPoint, Vec2 oldPoint) {
	if (disabled)
		return;

	position += newPoint - oldPoint;
	anchor = nullptr;
}

void Frame::render() {
	if (visible) {

		Vec4 blendColor = (disabled) ? GUI::COLOR::DISABLED : GUI::COLOR::WHITE;

		resize();

		// TitleBar
		Vec2f titleBarPosition = position;
		Vec2f titleBarDimension = Vec2f(width, titleBarHeight);
		Path::rectFilled(titleBarPosition, titleBarDimension, 0, GUI::COLOR::blend(titleBarColor, blendColor));

		// Buttons
		closeButton->render();
		minimizeButton->render();

		// Title
		if (!title->text.empty())
			title->render();

		if (!minimized) {
			// Padding
			Vec2f offsetPosition = titleBarPosition + Vec2f(0, -titleBarHeight);
			Vec2f offsetDimension = dimension + Vec2f(0, -titleBarHeight);
			Vec2f xRange = Vec2f(-GUI::screen->camera.aspect, GUI::screen->camera.aspect) * 2;
			Vec2f yRange = Vec2(-1, 1);
			Vec4f color = GUI::COLOR::blend(Vec4f(0.4, 0.4, 0.4, 1), blendColor);

			Path::rectUVRange(GUI::screen->blurFrameBuffer->texture->id, offsetPosition, offsetDimension, xRange, yRange, color);

			renderChildren();
		}

		// Outline
		//Vec2f outlinePosition = titleBarPosition;
		//Vec2f outlineDimension = dimension;
		//Path::rect(outlinePosition, outlineDimension, 0, GUI::COLOR::blend(GUI::COLOR::NAVY, blendColor));
	}
}
