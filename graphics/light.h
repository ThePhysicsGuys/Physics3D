#pragma once

#include "../physics/math/linalg/vec.h"

struct Attenuation {
	float constant;
	float linear;
	float exponent;
};

struct Light {
	Vec3f position;
	Color3 color;
	float intensity;
	Attenuation attenuation;

	Light(Vec3f position, Color3 color, float intensity, Attenuation attenuation) : position(position), color(color), intensity(intensity), attenuation(attenuation) {};
};

