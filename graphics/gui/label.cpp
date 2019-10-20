#include "core.h"

#include "label.h"

#include "gui.h"
#include "font.h"
#include "path/path.h"
#include "renderUtils.h"
#include "mesh/primitive.h"
#include "shader/shaderProgram.h"

Label::Label(std::string text, double x, double y) : Label(text, x, y, GUI::fontSize, GUI::fontColor) {};

Label::Label(std::string text, double x, double y, double size) : Label(text, x, y, size, GUI::fontColor) {};

Label::Label(std::string text, double x, double y, double size, Vec4 color) : Label(text, x, y, size, color, GUI::font) {};

Label::Label(std::string text, double x, double y, double scale, Vec4 color, Font* font) : Component(x, y) {
	this->font = font;
	this->text = text;
	this->scale = scale;
	this->padding = GUI::padding;
	this->margin = GUI::margin;
	this->foregroundColor = color;
	this->backgroundColor = GUI::labelBackgroundColor;
};

void Label::render() {
	if (visible) {

		Vec4 blendColor = (disabled) ? GUI::COLOR::DISABLED : GUI::COLOR::WHITE;

		resize();
		
		Vec2 textPosition = position + Vec2(padding, -dimension.y + padding);
		Path::text(font, text, scale, textPosition, GUI::COLOR::blend(foregroundColor, blendColor));

		if (debug) {
			Path::rect(position, dimension, 0.0f, GUI::COLOR::R);
			Path::rect(position + Vec2f(padding, -padding), dimension - Vec2f(padding) * 2, 0.0f, GUI::COLOR::G);
		}
	}
}

Vec2 Label::resize() {
	if (resizing) {
		Vec2 fontDimension = font->size(text, scale);
		dimension = fontDimension + Vec2(padding) * 2;
	}

	return dimension;
}