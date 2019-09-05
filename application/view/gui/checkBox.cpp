#include "core.h"

#include "checkBox.h"

#include "label.h"
#include "../path/path.h"

#include "../texture.h"
#include "../shaderProgram.h"
#include "../renderUtils.h"

#include "../mesh/primitive.h"


CheckBox::CheckBox(std::string text, double x, double y, double width, double height, bool textured) : Component(x, y, width, height) {
	this->label = new Label(text, x, y);
	this->checkBoxLabelOffset = GUI::checkBoxLabelOffset;
	this->textured = textured;
	this->padding = GUI::padding;
	this->margin = GUI::margin;

	if (textured) {
		this->checkedTexture = GUI::checkBoxCheckedTexture;
		this->uncheckedTexture = GUI::checkBoxUncheckedTexture;
		this->hoverCheckedTexture = GUI::checkBoxHoverCheckedTexture;
		this->hoverUncheckedTexture = GUI::checkBoxHoverUncheckedTexture;
		this->pressCheckedTexture = GUI::checkBoxPressCheckedTexture;
		this->pressUncheckedTexture = GUI::checkBoxPressUncheckedTexture;
	} else {
		// TODO colors
	}
}

CheckBox::CheckBox(std::string text, double x, double y, bool textured) : CheckBox(text, x, y, GUI::checkBoxSize + 2 * padding, GUI::checkBoxSize + 2 * padding, textured) {

}

CheckBox::CheckBox(double x, double y, bool textured) : CheckBox("", x, y, GUI::checkBoxSize + 2 * padding, GUI::checkBoxSize + 2 * padding, textured) {

}

CheckBox::CheckBox(double x, double y, double width, double height, bool textured) : CheckBox("", x, y, width, height, textured) {

}

void CheckBox::render() {
	if (visible) {
		resize();

		Vec4f blendColor = (disabled) ? GUI::COLOR::DISABLED : GUI::COLOR::WHITE;

		Vec2f checkBoxPosition;
		Vec2f checkBoxDimension;
		
		if (label->text.empty()) {
			// No text, resizing using default width
			checkBoxPosition = position + Vec2(padding, -padding);
			checkBoxDimension = Vec2(GUI::checkBoxSize);
		} else {
			if (label->dimension.y > GUI::checkBoxSize) {
				// Label defines component size, checkbox is centered vertically
				checkBoxPosition = position + Vec2(padding, -padding - (label->dimension.y - GUI::checkBoxSize) / 2);
				checkBoxDimension = Vec2(GUI::checkBoxSize);
			} else {
				// Checkbox defines component size
				checkBoxPosition = position + Vec2(padding, -padding);
				checkBoxDimension = Vec2(GUI::checkBoxSize);
			}
		}

		if (pressed)
			if (textured)
				if (checked)
					Path::rectUV(pressCheckedTexture->getID(), checkBoxPosition, checkBoxDimension);
				else
					Path::rectUV(pressUncheckedTexture->getID(), checkBoxPosition, checkBoxDimension);
			else
				if (checked)
					Path::rectFilled(checkBoxPosition, checkBoxPosition, 0.0f, GUI::COLOR::blend(pressCheckedColor, blendColor));
				else
					Path::rectFilled(checkBoxPosition, checkBoxPosition, 0.0f, GUI::COLOR::blend(pressUncheckedColor, blendColor));
		else if (hovering)
			if (textured)
				if (checked)
					Path::rectUV(checkedTexture->getID(), checkBoxPosition, checkBoxDimension);
				else
					Path::rectUV(uncheckedTexture->getID(), checkBoxPosition, checkBoxDimension);
			else
				if (checked)
					Path::rectFilled(checkBoxPosition, checkBoxPosition, 0.0f, GUI::COLOR::blend(checkedColor, blendColor));
				else
					Path::rectFilled(checkBoxPosition, checkBoxPosition, 0.0f, GUI::COLOR::blend(uncheckedColor, blendColor));
		else
			if (textured)
				if (checked)
					Path::rectUV(hoverCheckedTexture->getID(), checkBoxPosition, checkBoxDimension);
				else
					Path::rectUV(hoverUncheckedTexture->getID(), checkBoxPosition, checkBoxDimension);
			else
				if (checked)
					Path::rectFilled(checkBoxPosition, checkBoxPosition, 0.0f, GUI::COLOR::blend(hoverCheckedColor, blendColor));
				else
					Path::rectFilled(checkBoxPosition, checkBoxPosition, 0.0f, GUI::COLOR::blend(hoverUncheckedColor, blendColor));

		if (!label->text.empty()) {
			Vec2 labelPosition;
			if (label->dimension.y > GUI::checkBoxSize) {
				// Label defines the size of the component
				labelPosition = position + Vec2(padding + checkBoxDimension.x + checkBoxLabelOffset, -padding);
			} else {
				// Label is centered vertically
				labelPosition = position + Vec2(padding + checkBoxDimension.x + checkBoxLabelOffset, -padding - (GUI::checkBoxSize - label->dimension.y) / 2);
			}
			label->position = labelPosition;
			label->render();
		}

		if (debug) {
			Path::rect(position, dimension, 0.0f, GUI::COLOR::RED);
			Path::rect(checkBoxPosition, checkBoxDimension, 0.0f, GUI::COLOR::GREEN);
		}
	}
}

Vec2 CheckBox::resize() {
	if (!label->text.empty()) {
		label->resize();
		if (label->dimension.y > GUI::checkBoxSize) {
			// Label defines dimension
			dimension = label->dimension + Vec2(GUI::checkBoxSize + checkBoxLabelOffset, 0) + Vec2(padding) * 2;
		} else {
			// Checkbox defines dimension
			dimension = Vec2(GUI::checkBoxSize + checkBoxLabelOffset + label->width, GUI::checkBoxSize) + Vec2(padding) * 2;
		}
	}

	return dimension;
}

void CheckBox::disable() {
	disabled = true;
	label->disable();
}

void CheckBox::enable() {
	disabled = false;
	label->enable();
}

void CheckBox::enter() {
	if (disabled)
		return;

	hovering = true;
}

void CheckBox::exit() {
	if (disabled)
		return;

	hovering = false;
	pressed = false;
}

void CheckBox::press(Vec2 point) {
	if (disabled)
		return;

	pressed = true;
}

void CheckBox::release(Vec2 point) {
	if (disabled)
		return;

	pressed = false;
	checked = !checked;
	(*action)(this);
}
