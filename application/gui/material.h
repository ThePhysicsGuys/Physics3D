#pragma once

#include "../engine/math/vec4.h"
#include "texture.h"

struct Material {
	Texture* texture;

	Vec3f ambient;
	Vec3f diffuse;
	Vec3f specular;
	float reflectance;

	Material(Vec3f ambient, Vec3f diffuse, Vec3f specular, float reflectance) : ambient(ambient), diffuse(diffuse), specular(specular), reflectance(reflectance), texture(nullptr) {};

	Material(Vec3f ambient, Vec3f diffuse, Vec3f specular, float reflectance, Texture* texture) : ambient(ambient), diffuse(diffuse), specular(specular), reflectance(reflectance), texture(texture) {};

	void setTexture(Texture* texture) {
		this->texture = texture;
	}
};