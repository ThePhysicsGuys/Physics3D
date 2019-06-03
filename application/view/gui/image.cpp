#include "image.h"

#include "gui.h"

Image::Image(double x, double y, double width, double height) : Component(x, y, width, height) {
	Vec2 dimension = GUI::unmap(Vec2(width, height));
	this->texture = new Texture((unsigned int) dimension.x, (unsigned int) dimension.y);
}

Image::Image(double x, double y, Texture* texture) : Component(x, y) {
	this->texture = texture;
};

Image::Image(double x, double y, double width, double height, Texture* texture) : Component(x, y, width, height) {
	this->texture = texture;
}

void Image::render() {
	if (visible) {
		if (texture) GUI::shader->update(texture);
		else GUI::shader->update(GUI::COLOR::BLACK);
		GUI::quad->resize(position, dimension);
		GUI::quad->render();
	}
}

Vec2 Image::resize() {
	if (resizing)
		dimension = Vec2(texture->width, texture->height);
	return dimension;
}