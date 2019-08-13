#pragma once

#include "../engine/math/vec.h"

struct Attenuation {
	float constant;
	float linear;
	float exponent;
};

struct Light {
	Vec3f position;
	Vec3f color;
	float intensity;
	Attenuation attenuation;

	Light(Vec3f position, Vec3f color, float intensity, Attenuation attenuation) : position(position), color(color), intensity(intensity), attenuation(attenuation) {};
};

