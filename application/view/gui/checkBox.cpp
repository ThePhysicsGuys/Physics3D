#include "CheckBox.h"

CheckBox::CheckBox(double x, double y, double width, double height, bool textured) : Component(x, y, width, height) {
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

CheckBox::CheckBox(double x, double y, bool textured) : CheckBox(x, y, GUI::defaultCheckBoxSize, GUI::defaultCheckBoxSize, textured) {}

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

void CheckBox::renderCheck() {
	if (textured) {
		GUI::defaultShader->update(checkedColor);
	} else {
		GUI::defaultShader->update(checkedTexture);
	}

	Vec2 checkPosition = position + Vec2(checkOffset, -checkOffset);
	Vec2 checkDimension = dimension - Vec2(checkOffset) * 2;

	GUI::defaultQuad->resize(checkPosition, checkDimension);
	GUI::defaultQuad->render();
}

void CheckBox::render() {
	resize();

	GUI::defaultQuad->resize(position, dimension);

	if (pressed)
		renderPressed();
	else if (hovering)
		renderHovering();
	else
		renderIdle();

	if (checked)
		renderCheck();
}

Vec2 CheckBox::resize() {
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
