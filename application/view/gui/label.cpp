#include "label.h"
#include "gui.h"

Label::Label(std::string text, double x, double y) : Label(text, x, y, GUI::defaultFontSize, GUI::defaultFontColor) {};

Label::Label(std::string text, double x, double y, double size) : Label(text, x, y, size, GUI::defaultFontColor) {};

Label::Label(std::string text, double x, double y, double size, Vec4 color) : Label(text, x, y, size, color, GUI::defaultFont) {};

Label::Label(std::string text, double x, double y, double scale, Vec4 color, Font* font) : Component(x, y) {
	this->font = font;
	this->text = text;
	this->scale = scale;
	this->padding = GUI::defaultLabelPadding;
	this->margin = GUI::defaultLabelMargin;
	this->foregroundColor = color;
	this->backgroundColor = GUI::defaultLabelBackgroundColor;
};

void Label::render() {
	if (visible) {
		resize();
		Vec2 textPosition = position + Vec2(0, -dimension.y);
		font->render(text, textPosition, foregroundColor, scale);
	}
}

Vec2 Label::resize() {
	if (resizing)
		dimension = font->size(text, scale);
	return dimension;
}