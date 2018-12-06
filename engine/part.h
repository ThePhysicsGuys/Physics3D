#pragma once

#include "geometry/shape.h"

struct Part {
public:
	Shape hitbox;
	struct {
		double density;
		double friction;
	} properties;

	Part();
	Part(Shape s, double density, double friction);
};
