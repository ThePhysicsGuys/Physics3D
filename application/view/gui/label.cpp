#include "label.h"
#include "gui.h"

Label::Label(std::string text, double x, double y) : Label(text, x, y, GUI::defaultFontSize, GUI::defaultFontColor) {};

Label::Label(std::string text, double x, double y, double size) : Label(text, x, y, size, GUI::defaultFontColor) {};

Label::Label(std::string text, double x, double y, double size, Vec4 color) : Label(text, x, y, size, color, GUI::defaultFont) {};

Label::Label(std::string text, double x, double y, double scale, Vec4 color, Font* font) : Component(x, y) {
	this->font = font;
	this->text = text;
	this->scale = scale;
	this->padding = GUI::defaultPadding;
	this->margin = GUI::defaultMargin;
	this->foregroundColor = color;
	this->backgroundColor = GUI::defaultLabelBackgroundColor;
};

void Label::render() {
	if (visible) {
		resize();

		Vec2 fontPosition = position + Vec2(padding, -dimension.y+ padding);
		font->render(text, fontPosition, foregroundColor, scale);

		GUI::defaultQuad->resize(position, dimension);
		GUI::defaultShader->update(GUI::COLOR::RED);
		GUI::defaultQuad->render(GL_LINE);

		GUI::defaultQuad->resize(position + Vec2(padding, -padding), dimension - Vec2(padding) * 2);
		GUI::defaultShader->update(GUI::COLOR::GREEN);
		GUI::defaultQuad->render(GL_LINE);
	}
}

Vec2 Label::resize() {
	if (resizing) {
		Vec2 fontDimension = font->size(text, scale);
		dimension = fontDimension + Vec2(padding) * 2;
	}

	return dimension;
}