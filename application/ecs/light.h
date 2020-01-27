#pragma once

#include "../physics/math/linalg/vec.h"

#include "../engine/ecs/component.h"

namespace Application {

struct Attenuation {
	float constant;
	float linear;
	float exponent;
};

struct Light : public Component {
	DEFINE_COMPONENT(Light, true);

	Vec3f position;
	Color3 color;
	float intensity;
	Attenuation attenuation;

	Light(Vec3f position, Color3 color, float intensity, Attenuation attenuation) : position(position), color(color), intensity(intensity), attenuation(attenuation) {};
};

};