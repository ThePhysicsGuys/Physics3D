#include "label.h"
#include "gui.h"

Label::Label(std::string text, double x, double y) : Label(text, x, y, GUI::defaultFontSize, GUI::defaultFontColor) {};

Label::Label(std::string text, double x, double y, double size) : Label(text, x, y, size, GUI::defaultFontColor) {};

Label::Label(std::string text, double x, double y, double size, Vec4 color) : Label(text, x, y, size, color, GUI::defaultFont) {};

Label::Label(std::string text, double x, double y, double scale, Vec4 color, Font* font) : Component(Vec2(x, y)) {
	this->font = font;
	this->text = text;
	this->position = Vec2(x, y);
	this->scale = scale;
	this->offset = GUI::defaultLabelOffset;
	this->foregroundColor = color;
	this->backgroundColor = GUI::defaultLabelBackgroundColor;
};

void Label::render() {
	Vec2 labelPosition = Vec2(position.x - offset, position.y + offset);
	Vec2 labelSize = dimension + 2 * offset;

	// Background
	//GUI::defaultShader->update(backgroundColor);
	//GUI::defaultQuad->resize(labelPosition, labelSize);
	//GUI::defaultQuad->render();

	// Text
	Vec2 textPosition = position;
	if (layout == Layout::ABSOLUTE) {

	}
	if (layout == Layout::RELATIVE) {

	}
	font->render(text, interpolate(), foregroundColor, scale);
}

Vec2 Label::resize() {
	if (resizing)
		dimension = font->size(text, scale);
	return dimension;
}