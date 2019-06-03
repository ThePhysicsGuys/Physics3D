#include "CheckBox.h"

CheckBox::CheckBox(std::string text, double x, double y, double width, double height, bool textured) : Component(x, y, width, height) {
	this->label = new Label(text, x, y);
	this->checkBoxLabelOffset = GUI::checkBoxLabelOffset;
	this->textured = textured;

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

CheckBox::CheckBox(std::string text, double x, double y, bool textured) : CheckBox(text, x, y, GUI::checkBoxSize, GUI::checkBoxSize, textured) {}

CheckBox::CheckBox(double x, double y, bool textured) : CheckBox("", x, y, GUI::checkBoxSize, GUI::checkBoxSize, textured) {}

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

		if (!label->text.empty())
			GUI::quad->resize(position + Vec2(label->padding, -label->padding), Vec2(dimension.y - 2 * label->padding));
		else
			GUI::quad->resize(position, dimension);

		if (pressed)
			renderPressed();
		else if (hovering)
			renderHovering();
		else
			renderIdle();

		if (!label->text.empty()) {
			Vec2 labelPosition = position + Vec2(dimension.y + checkBoxLabelOffset, 0);
			label->position = labelPosition;
			label->render();
		}

		if (debug) {
			GUI::quad->resize(position, dimension);
			GUI::shader->update(GUI::COLOR::RED);
			GUI::quad->render(GL_LINE);

			GUI::quad->resize(position + Vec2(label->padding, -label->padding), dimension - Vec2(label->padding) * 2);
			GUI::shader->update(GUI::COLOR::GREEN); 
			GUI::quad->render(GL_LINE);
		}
	}
}

Vec2 CheckBox::resize() {
	if (!label->text.empty()) {
		Vec2 labelDimension = label->resize();
		dimension = labelDimension + Vec2(labelDimension.y + checkBoxLabelOffset, 0);
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
