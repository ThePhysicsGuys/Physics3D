#pragma once

#include "../engine/math/vec4.h"

struct Material {
	Vec4f ambient;
	Vec4f diffuse;
	Vec4f specular;
	float reflectance;

	Material(Vec4f ambient, Vec4f diffuse, Vec4f specular, float reflectance) : ambient(ambient), diffuse(diffuse), specular(specular), reflectance(reflectance) {};
};