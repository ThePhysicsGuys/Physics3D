#include "button.h"

Button::Button(double x, double y, double width, double height, bool textured) : Component(x, y, width, height) {
	this->textured = textured;
}

Button::Button(std::string text, double x, double y, bool textured) : Component(x, y) {
	this->fontColor = GUI::defaultFontColor;
	this->fontSize = GUI::defaultFontSize;
	this->textured = textured;
	this->text = text;
}

void Button::renderPressed() {
	if (textured) {
		GUI::defaultShader->update(pressTexture);
	} else {
		GUI::defaultShader->update(pressColor);
	}

	GUI::defaultQuad->render();
}

void Button::renderHovering() {
	if (textured) {
		GUI::defaultShader->update(hoverTexture);
	} else {
		GUI::defaultShader->update(hoverColor);
	}

	GUI::defaultQuad->render();
}

void Button::renderIdle() {
	if (textured) {
		GUI::defaultShader->update(idleTexture);
	} else {
		GUI::defaultShader->update(idleColor);
	}

	GUI::defaultQuad->render();
}

void Button::render() {
	resize();

	GUI::defaultQuad->resize(position, dimension);

	if (pressed)
		renderPressed();
	else if (hovering)
		renderHovering();
	else 
		renderIdle();

	if (!text.empty())
		GUI::defaultFont->render(text, position, fontColor, fontSize);
}

Vec2 Button::resize() {
	if (resizing) {
		dimension = GUI::defaultFont->size(text, fontSize);
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
