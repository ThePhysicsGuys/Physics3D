#include "core.h"

#include "material.h"

#include "../graphics/texture.h"

namespace Application {

void Material::setTexture(Graphics::Texture* texture) {
	texture->setUnit(0);
	this->texture = texture;
}

void Material::setNormalMap(Graphics::Texture* normalMap) {
	normalMap->setUnit(1);
	this->normal = normalMap;
}

bool Material::operator==(const Material& other) const {
	return
		other.ambient == ambient &&
		other.diffuse == diffuse &&
		other.specular == specular &&
		other.reflectance == reflectance &&
		other.texture == texture &&
		other.normal == normal;
}

};