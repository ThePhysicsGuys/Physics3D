#include "button.h"

Button::Button(double x, double y, double width, double height, bool textured) : Component(x, y, width, height) {
	this->textured = textured;
}

Button::Button(std::string text, double x, double y, bool textured) : Component(x, y) {
	this->fontColor = GUI::fontColor;
	this->fontSize = GUI::fontSize;
	this->textured = textured;
	this->text = text;
}

void Button::renderPressed() {
	if (textured) {
		GUI::shader->update(pressTexture);
	} else {
		GUI::shader->update(pressColor);
	}

	GUI::quad->render();
}

void Button::renderHovering() {
	if (textured) {
		GUI::shader->update(hoverTexture);
	} else {
		GUI::shader->update(hoverColor);
	}

	GUI::quad->render();
}

void Button::renderIdle() {
	if (textured) {
		GUI::shader->update(idleTexture);
	} else {
		GUI::shader->update(idleColor);
	}

	GUI::quad->render();
}

void Button::render() {
	resize();

	GUI::quad->resize(position, dimension);

	if (pressed)
		renderPressed();
	else if (hovering)
		renderHovering();
	else 
		renderIdle();

	if (!text.empty())
		GUI::font->render(text, position, fontColor, fontSize);
}

Vec2 Button::resize() {
	if (resizing) {
		dimension = GUI::font->size(text, fontSize);
	} 

	return dimension;
}

void Button::enter() {
	hovering = true;
}

void Button::exit() {
	hovering = false;
	pressed = false;
}

void Button::press(Vec2 point) {
	pressed = true;
}

void Button::release(Vec2 point) {
	pressed = false;
	(*action)(this);
}
