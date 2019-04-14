#pragma once

struct Part;
struct Physical;
#include "geometry/shape.h"

struct Part {
public:
	Physical* parent;
	NormalizedShape hitbox;
	Shape transformed;
	double maxRadius;
	CFrame cframe;
	size_t partIndex = -1;
	struct {
		double density;
		double friction;
	} properties;
	int drawMeshId = 0;

	Part() = default;
	Part(NormalizedShape shape, CFrame position, double density, double friction);
	Part(Shape shape, CFrame position, double density, double friction);
};

#include "physical.h"
