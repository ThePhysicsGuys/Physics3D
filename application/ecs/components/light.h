#pragma once

#include "../physics/math/linalg/vec.h"

#include "component.h"

namespace Application {

struct Attenuation {
	float constant;
	float linear;
	float exponent;
};

struct Light : public Component {
	Vec3f position;
	Color3 color;
	float intensity;
	Attenuation attenuation;

	Light(Vec3f position, Color3 color, float intensity, Attenuation attenuation) : position(position), color(color), intensity(intensity), attenuation(attenuation) {};

	/*
		Returns whether an Entity can contain multiple instances of this Component
	*/
	virtual bool isUnique() const {
		return true;
	}
};

};