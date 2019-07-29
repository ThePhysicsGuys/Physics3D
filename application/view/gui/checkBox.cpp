#include "CheckBox.h"

#include "label.h"

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

CheckBox::CheckBox(std::string text, double x, double y, bool textured) : CheckBox(text, x, y, GUI::checkBoxSize + 2 * padding, GUI::checkBoxSize + 2 * padding, textured) {}

CheckBox::CheckBox(double x, double y, bool textured) : CheckBox("", x, y, GUI::checkBoxSize + 2 * padding, GUI::checkBoxSize + 2 * padding, textured) {}

CheckBox::CheckBox(double x, double y, double width, double height, bool textured) : CheckBox("", x, y, width, height, textured) {}


void CheckBox::renderPressed(Vec4 blendColor) {
	if (textured) {
		if (checked) {
			Shaders::quadShader.updateTexture(pressCheckedTexture, blendColor);
		} else {
			Shaders::quadShader.updateTexture(pressUncheckedTexture, blendColor);
		}
	} else {
		if (checked) {
			Shaders::quadShader.updateColor(GUI::COLOR::blend(pressCheckedColor, blendColor));
		} else {
			Shaders::quadShader.updateColor(GUI::COLOR::blend(pressUncheckedColor, blendColor));
		}
	}

	GUI::quad->render();
}

void CheckBox::renderHovering(Vec4 blendColor) {
	if (textured) {
		if (checked) {
			Shaders::quadShader.updateTexture(hoverCheckedTexture, blendColor);
		} else {
			Shaders::quadShader.updateTexture(hoverUncheckedTexture, blendColor);
		}
	} else {
		if (checked) {
			Shaders::quadShader.updateColor(GUI::COLOR::blend(hoverCheckedColor, blendColor));
		} else {
			Shaders::quadShader.updateColor(GUI::COLOR::blend(hoverUncheckedColor, blendColor));
		}
	}

	GUI::quad->render();
}

void CheckBox::renderIdle(Vec4 blendColor) {
	if (textured) {
		if (checked) {
			Shaders::quadShader.updateTexture(checkedTexture, blendColor);
		} else {
			Shaders::quadShader.updateTexture(uncheckedTexture, blendColor);
		}
	} else {
		if (checked) {
			Shaders::quadShader.updateColor(GUI::COLOR::blend(checkedColor, blendColor));
		} else {
			Shaders::quadShader.updateColor(GUI::COLOR::blend(uncheckedColor, blendColor));
		}
	}

	GUI::quad->render();
}

void CheckBox::render() {
	if (visible) {
		resize();

		Vec4 blendColor = (disabled) ? GUI::COLOR::DISABLED : GUI::COLOR::WHITE;

		Vec2 checkBoxPosition;
		Vec2 checkBoxDimension;
		
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

		GUI::quad->resize(checkBoxPosition, checkBoxDimension);

		if (pressed)
			renderPressed(blendColor);
		else if (hovering)
			renderHovering(blendColor);
		else
			renderIdle(blendColor);

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
			GUI::quad->resize(position, dimension);
			Shaders::quadShader.updateColor(GUI::COLOR::RED);
			GUI::quad->render(Renderer::WIREFRAME);

			GUI::quad->resize(checkBoxPosition, checkBoxDimension);
			Shaders::quadShader.updateColor(GUI::COLOR::GREEN);
			GUI::quad->render(Renderer::WIREFRAME);
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
