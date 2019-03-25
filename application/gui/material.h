#pragma once

#include "../engine/math/vec4.h"

struct Material {
	Vec3f ambient;
	Vec3f diffuse;
	Vec3f specular;
	float reflectance;

	Material(Vec3f ambient, Vec3f diffuse, Vec3f specular, float reflectance) : ambient(ambient), diffuse(diffuse), specular(specular), reflectance(reflectance) {};
};