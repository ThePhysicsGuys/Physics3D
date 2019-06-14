#include "CheckBox.h"

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


void CheckBox::renderPressed() {
	if (textured) {
		if (checked) {
			GUI::shader->update(pressCheckedTexture);
		} else {
			GUI::shader->update(pressUncheckedTexture);
		}
	} else {
		if (checked) {
			GUI::shader->update(pressCheckedColor);
		} else {
			GUI::shader->update(pressUncheckedColor);
		}
	}

	GUI::quad->render();
}

void CheckBox::renderHovering() {
	if (textured) {
		if (checked) {
			GUI::shader->update(hoverCheckedTexture);
		} else {
			GUI::shader->update(hoverUncheckedTexture);
		}
	} else {
		if (checked) {
			GUI::shader->update(hoverCheckedColor);
		} else {
			GUI::shader->update(hoverUncheckedColor);
		}
	}

	GUI::quad->render();
}

void CheckBox::renderIdle() {
	if (textured) {
		if (checked) {
			GUI::shader->update(checkedTexture);
		} else {
			GUI::shader->update(uncheckedTexture);
		}
	} else {
		if (checked) {
			GUI::shader->update(checkedColor);
		} else {
			GUI::shader->update(uncheckedColor);
		}
	}

	GUI::quad->render();
}

void CheckBox::render() {
	if (visible) {
		resize();

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
			renderPressed();
		else if (hovering)
			renderHovering();
		else
			renderIdle();

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
			GUI::shader->update(GUI::COLOR::RED);
			GUI::quad->render(GL_LINE);

			GUI::quad->resize(checkBoxPosition, checkBoxDimension);
			GUI::shader->update(GUI::COLOR::GREEN); 
			GUI::quad->render(GL_LINE);
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

void CheckBox::enter() {
	hovering = true;
}

void CheckBox::exit() {
	hovering = false;
	pressed = false;
}

void CheckBox::press(Vec2 point) {
	pressed = true;
}

void CheckBox::release(Vec2 point) {
	pressed = false;
	checked = !checked;
	(*action)(this);
}
