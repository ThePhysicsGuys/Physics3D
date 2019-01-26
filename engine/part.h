#pragma once

#include "geometry/shape.h"

struct Part {
public:
	NormalizedShape hitbox;
	CFrame cframe;
	int drawMeshId = 0;
	struct {
		double density;
		double friction;
	} properties;

	// Part();
	Part(NormalizedShape s, CFrame position, double density, double friction);
	Part(Shape s, CFrame position, double density, double friction);
};
