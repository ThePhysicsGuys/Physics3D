#include "core.h"

#include "image.h"

#include "texture.h"
#include "guiUtils.h"
#include "path/path.h"
#include "shader/shaderProgram.h"
#include "mesh/primitive.h"


Image::Image(double x, double y, double width, double height) : Component(x, y, width, height) {
	Vec2 dimension = GUI::unmap(Vec2(width, height));
	this->texture = nullptr;
	this->margin = GUI::margin;
	this->padding = GUI::padding;
	this->fixedWidth = true;
	this->fixedHeight = true;
}

Image::Image(double x, double y, Texture* texture) : Component(x, y) {
	this->texture = texture;
	this->margin = GUI::margin;
	this->padding = GUI::padding;
	this->fixedWidth = false;
	this->fixedHeight = false;
};

Image::Image(double x, double y, double width, double height, Texture* texture) : Component(x, y, width, height) {
	this->texture = texture;
	this->margin = GUI::margin;
	this->padding = GUI::padding;
	this->fixedWidth = true;
	this->fixedHeight = true;
}

void Image::render() {
	if (!texture)
		return;

	if (visible) {
		Color blendColor = (disabled) ? COLOR::DISABLED : COLOR::WHITE;

		if (texture)
			Path::rectUV(texture->getID(), position, dimension);
		else 
			Path::rectFilled(position, dimension, 0, COLOR::blend(COLOR::BLACK, blendColor));

		Path::rect(position, dimension, 0.0f, COLOR::R);
	}
}

Vec2 Image::resize() {
	if (!texture)
		return dimension;

	if (resizing) {
		if (fixedWidth && fixedHeight)
			return dimension;

		dimension -= Vec2(GUI::padding) * 2;

		if (fixedWidth) 
			this->height = this->width / texture->getAspect();
		else if (fixedHeight)
			this->width = this->height * texture->getAspect();
		else
			dimension = Vec2(texture->getWidth(), texture->getHeight());

		dimension += Vec2(GUI::padding) * 2;
	}

	return dimension;
}

Image* Image::fixHeight(float height) {
	fixedHeight = true;
	this->height = height;

	return this;
}

Image* Image::fixWidth(float width) {
	fixedWidth = true;
	this->width = width;

	return this;
}