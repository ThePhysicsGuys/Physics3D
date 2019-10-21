#include "core.h"

#include "material.h"

#include "texture.h"

void Material::setTexture(Texture* texture) {
	texture->setUnit(0);
	this->texture = texture;
}

void Material::setNormalMap(Texture* normalMap) {
	normalMap->setUnit(1);
	this->normal = normalMap;
}