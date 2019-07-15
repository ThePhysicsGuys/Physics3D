#include "button.h"

#include "../shaderProgram.h"

Button::Button(double x, double y, double width, double height, bool textured) : Component(x, y, width, height) {
	this->textured = textured;
	this->borderColor = GUI::borderColor;

	this->padding = GUI::padding;
	this->margin = GUI::margin;
	
	if (textured)
		this->borderWidth = 0;
	else
		this->borderWidth = GUI::borderWidth;
}

Button::Button(std::string text, double x, double y, bool textured) : Component(x, y) {
	this->fontColor = GUI::fontColor;
	this->fontSize = GUI::fontSize;
	this->textured = textured;

	this->padding = GUI::padding;
	this->margin = GUI::margin;

	this->text = text;
	this->borderColor = GUI::borderColor;
	if (textured)
		this->borderWidth = 0;
	else
		this->borderWidth = GUI::borderWidth;
}

void Button::renderPressed(Vec4 blendColor) {

	if (textured) {
		Shaders::quadShader.updateTexture(pressTexture, blendColor);
	} else {
		Shaders::quadShader.updateColor(GUI::COLOR::blend(pressColor, blendColor));
	}

	GUI::quad->render();
}

void Button::renderHovering(Vec4 blendColor) {

	if (textured) {
		Shaders::quadShader.updateTexture(hoverTexture, blendColor);
	} else {
		Shaders::quadShader.updateColor(GUI::COLOR::blend(hoverColor, blendColor));
	}

	GUI::quad->render();
}

void Button::renderIdle(Vec4 blendColor) {

	if (textured) {
		Shaders::quadShader.updateTexture(idleTexture, blendColor);
	} else {
		Shaders::quadShader.updateColor(GUI::COLOR::blend(idleColor, blendColor));
	}

	GUI::quad->render();
}

void Button::render() {
	if (visible) {
		resize();

		Vec4 blendColor = (disabled) ? GUI::COLOR::DISABLED : GUI::COLOR::WHITE;

		GUI::quad->resize(position, dimension);

		if (borderWidth > 0) {
			Shaders::quadShader.updateColor(GUI::COLOR::blend(borderColor, blendColor));
			GUI::quad->render();
			GUI::quad->resize(position + Vec2(borderWidth, -borderWidth), dimension - Vec2(borderWidth) * 2);
		}

		if (pressed)
			renderPressed(blendColor);
		else if (hovering)
			renderHovering(blendColor);
		else
			renderIdle(blendColor);

		if (!text.empty())
			GUI::font->render(text, position + Vec2(borderWidth, -borderWidth), GUI::COLOR::blend(fontColor, blendColor), fontSize);
	}
}

Vec2 Button::resize() {
	if (resizing) {
		dimension = GUI::font->size(text, fontSize) + Vec2(borderWidth) * 2;
	} 

	return dimension;
}

void Button::enter() {
	if (disabled)
		return;

	hovering = true;
}

void Button::exit() {
	if (disabled)
		return;

	hovering = false;
	pressed = false;
}

void Button::press(Vec2 point) {
	if (disabled)
		return;

	pressed = true;
}

void Button::release(Vec2 point) {
	if (disabled)
		return;

	pressed = false;
	(*action)(this);
}
