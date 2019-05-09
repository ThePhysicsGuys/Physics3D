#include "CheckBox.h"

CheckBox::CheckBox(std::string text, double x, double y, double width, double height, bool textured) : Component(x, y, width, height) {
	this->label = new Label(text, x, y);
	this->checkBoxLabelOffset = GUI::defaultCheckBoxLabelOffset;
	this->textured = textured;

	if (textured) {
		this->checkedTexture = GUI::defaultCheckBoxCheckedTexture;
		this->uncheckedTexture = GUI::defaultCheckBoxUncheckedTexture;
		this->hoverCheckedTexture = GUI::defaultCheckBoxHoverCheckedTexture;
		this->hoverUncheckedTexture = GUI::defaultCheckBoxHoverUncheckedTexture;
		this->pressCheckedTexture = GUI::defaultCheckBoxPressCheckedTexture;
		this->pressUncheckedTexture = GUI::defaultCheckBoxPressUncheckedTexture;
	} else {
		// TODO colors
	}
}

CheckBox::CheckBox(std::string text, double x, double y, bool textured) : CheckBox(text, x, y, GUI::defaultCheckBoxSize, GUI::defaultCheckBoxSize, textured) {}

CheckBox::CheckBox(double x, double y, bool textured) : CheckBox("", x, y, GUI::defaultCheckBoxSize, GUI::defaultCheckBoxSize, textured) {}

CheckBox::CheckBox(double x, double y, double width, double height, bool textured) : CheckBox("", x, y, width, height, textured) {}


void CheckBox::renderPressed() {
	if (textured) {
		if (checked) {
			GUI::defaultShader->update(pressCheckedTexture);
		} else {
			GUI::defaultShader->update(pressUncheckedTexture);
		}
	} else {
		if (checked) {
			GUI::defaultShader->update(pressCheckedColor);
		} else {
			GUI::defaultShader->update(pressUncheckedColor);
		}
	}

	GUI::defaultQuad->render();
}

void CheckBox::renderHovering() {
	if (textured) {
		if (checked) {
			GUI::defaultShader->update(hoverCheckedTexture);
		} else {
			GUI::defaultShader->update(hoverUncheckedTexture);
		}
	} else {
		if (checked) {
			GUI::defaultShader->update(hoverCheckedColor);
		} else {
			GUI::defaultShader->update(hoverUncheckedColor);
		}
	}

	GUI::defaultQuad->render();
}

void CheckBox::renderIdle() {
	if (textured) {
		if (checked) {
			GUI::defaultShader->update(checkedTexture);
		} else {
			GUI::defaultShader->update(uncheckedTexture);
		}
	} else {
		if (checked) {
			GUI::defaultShader->update(checkedColor);
		} else {
			GUI::defaultShader->update(uncheckedColor);
		}
	}

	GUI::defaultQuad->render();
}

void CheckBox::render() {
	if (visible) {
		resize();

		if (!label->text.empty())
			GUI::defaultQuad->resize(position + Vec2(label->padding, -label->padding), Vec2(dimension.y - 2 * label->padding));
		else
			GUI::defaultQuad->resize(position, dimension);

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

		GUI::defaultQuad->resize(position, dimension);
		GUI::defaultShader->update(GUI::COLOR::RED);
		GUI::defaultQuad->render(GL_LINE);

		GUI::defaultQuad->resize(position + Vec2(label->padding, -label->padding), dimension - Vec2(label->padding) * 2);
		GUI::defaultShader->update(GUI::COLOR::GREEN);
		GUI::defaultQuad->render(GL_LINE);
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
