#include "material.h"

#include "texture.h"

void Material::setTexture(Texture* texture) {
	texture->unit = 0;
	this->texture = texture;
}

void Material::setNormalMap(Texture* normalMap) {
	normalMap->unit = 1;
	this->normal = normalMap;
}