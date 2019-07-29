#include "image.h"

#include "gui.h"

#include "../shaderProgram.h"
#include "../texture.h"

#include "../mesh/primitive.h"


Image::Image(double x, double y, double width, double height) : Component(x, y, width, height) {
	Vec2 dimension = GUI::unmap(Vec2(width, height));
	this->texture = new Texture((unsigned int) dimension.x, (unsigned int) dimension.y);
	this->margin = GUI::margin;
	this->padding = GUI::padding;
}

Image::Image(double x, double y, Texture* texture) : Component(x, y) {
	this->texture = texture;
	this->margin = GUI::margin;
	this->padding = GUI::padding;
};

Image::Image(double x, double y, double width, double height, Texture* texture) : Component(x, y, width, height) {
	this->texture = texture;
	this->margin = GUI::margin;
	this->padding = GUI::padding;
}

void Image::render() {
	if (visible) {

		Vec4 blendColor = (disabled) ? GUI::COLOR::DISABLED : GUI::COLOR::WHITE;

		if (texture) Shaders::quadShader.updateTexture(texture, blendColor);
		else Shaders::quadShader.updateColor(GUI::COLOR::blend(GUI::COLOR::BLACK, blendColor));
		GUI::quad->resize(position, dimension);
		GUI::quad->render();
	}
}

Vec2 Image::resize() {
	if (resizing)
		dimension = Vec2(texture->width, texture->height);
	return dimension;
}