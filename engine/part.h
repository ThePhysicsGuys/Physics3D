#pragma once

#include "geometry/shape.h"

struct Part {
public:
	Shape hitbox;
	int drawMeshId = 0;
	struct {
		double density;
		double friction;
	} properties;

	Part();
	Part(Shape s, double density, double friction);
};
