#include "core.h"

#include "image.h"

#include "texture.h"
#include "guiUtils.h"
#include "path/path.h"
#include "shader/shaderProgram.h"
#include "mesh/primitive.h"


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
		Color blendColor = (disabled) ? COLOR::DISABLED : COLOR::WHITE;

		if (texture)
			Path::rectUV(texture->getID(), position, dimension);
		else 
			Path::rectFilled(position, dimension, 0, COLOR::blend(COLOR::BLACK, blendColor));
	}
}

Vec2 Image::resize() {
	if (resizing)
		dimension = Vec2(texture->getWidth(), texture->getHeight());
	return dimension;
}