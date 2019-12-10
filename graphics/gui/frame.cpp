#include "core.h"

#include "frame.h"

#include "path/path.h"
#include "button.h"
#include "label.h"
#include "texture.h"
#include "shader/shaderProgram.h"
#include "renderUtils.h"
#include "guiUtils.h"
#include "mesh/primitive.h"
#include "buffers/frameBuffer.h"
#include "../physics/math/mathUtil.h"

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

void Frame::press(Vec2 point) {
	if (GUI::between(point.x - x, 0, GUI::frameResizeHandleSize)) {
		resizeFlags |= resizingW;
		resizing = false;
	}

	if (GUI::between(point.y - y, -GUI::frameResizeHandleSize, 0)) {
		resizeFlags |= resizingN;
		resizing = false;
	}

	if (GUI::between(point.x - x - width, -GUI::frameResizeHandleSize, 0)) {
		resizeFlags |= resizingE;
		resizing = false;
	}

	if (GUI::between(point.y - y + height, 0, GUI::frameResizeHandleSize)) {
		resizeFlags |= resizingS;
		resizing = false;
	}
}

void Frame::release(Vec2 point) {
	resizeFlags = None;
}

void Frame::drag(Vec2 newPoint, Vec2 oldPoint) {
	if (disabled)
		return;

	if (resizeFlags != None) {
		// component resizing should be off right now
		if (resizeFlags & resizingE) {
			width = GUI::clamp(newPoint.x - x, title->width, 10);
		} else if (resizeFlags & resizingW) {
			width = GUI::clamp(x - newPoint.x + width, title->width, 10);
			x = newPoint.x;
		}

		if (resizeFlags & resizingS) {
			height = GUI::clamp(y - newPoint.y, titleBarHeight, 10);
		} else if (resizeFlags & resizingN) {
			height = GUI::clamp(newPoint.y - y + height, titleBarHeight, 10);
			y = newPoint.y;
		}
	} else {
		position += newPoint - oldPoint;
		anchor = nullptr;
	}
}

void Frame::render() {
	if (visible) {

		Color blendColor = (disabled) ? COLOR::DISABLED : COLOR::WHITE;

		resize();

		// TitleBar
		Vec2f titleBarPosition = position;
		Vec2f titleBarDimension = Vec2f(width, titleBarHeight);
		Path::rectFilled(titleBarPosition, titleBarDimension, 0, COLOR::blend(titleBarColor, blendColor));

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
			Vec2f xRange = Vec2f(-GUI::windowInfo.aspect, GUI::windowInfo.aspect) * 2;
			Vec2f yRange = Vec2(-1, 1);
			Color color = COLOR::blend(Color(0.4, 0.4, 0.4, 1), blendColor);

			Path::rectUVRange(GUI::blurFrameBuffer->texture->getID(), offsetPosition, offsetDimension, xRange, yRange, color);

			renderChildren();
		}
	}
}
