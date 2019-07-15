#include "label.h"

#include "gui.h"

#include "../shaderProgram.h"
#include "../renderUtils.h"

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
		
		Vec2 fontPosition = position + Vec2(padding, -dimension.y + padding);
		font->render(text, fontPosition, GUI::COLOR::blend(foregroundColor, blendColor), scale);

		if (debug) {
			GUI::quad->resize(position, dimension);
			Shaders::quadShader.updateColor(GUI::COLOR::R);
			GUI::quad->render(Renderer::WIREFRAME);

			GUI::quad->resize(position + Vec2(padding, -padding), dimension - Vec2(padding) * 2);
			Shaders::quadShader.updateColor(GUI::COLOR::G);
			GUI::quad->render(Renderer::WIREFRAME);
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