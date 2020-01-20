#include "core.h"

#include "button.h"

#include "texture.h"
#include "font.h"
#include "path/path.h"

#include "mesh/primitive.h"


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

void Button::render() {
	if (visible) {
		resize();

		Color blendColor = (disabled) ? COLOR::DISABLED : COLOR::WHITE;

		Vec2f buttonPosition = position;
		Vec2f buttonDimension = dimension;

		if (borderWidth > 0) {
			Path::rectFilled(buttonPosition, buttonDimension, 0, COLOR::blend(borderColor, blendColor));

			buttonPosition = position + Vec2f(borderWidth, -borderWidth);
			buttonDimension = dimension - Vec2f(borderWidth) * 2;
		}

		if (pressed)
			if (textured) Path::rectUV(pressTexture->getID(), position, dimension);
			else Path::rectFilled(position, dimension, 0, COLOR::blend(pressColor, blendColor));
		else if (hovering)
			if (textured) Path::rectUV(hoverTexture->getID(), position, dimension);
			else Path::rectFilled(position, dimension, 0, COLOR::blend(hoverColor, blendColor));
		else
			if (textured) Path::rectUV(idleTexture->getID(), position, dimension);
			else Path::rectFilled(position, dimension, 0, COLOR::blend(idleColor, blendColor));

		if (!text.empty())
			Path::text(GUI::font, text, fontSize, position + Vec2(borderWidth, -borderWidth), COLOR::blend(fontColor, blendColor));
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

void Button::setColor(const Color& color) {
	idleColor = color;
	pressColor = color;
	hoverColor = color;
}