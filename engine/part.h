#pragma once

struct Part;
struct Physical;
#include "geometry/shape.h"

struct Part {
public:
	Physical* parent;
	NormalizedShape hitbox;
	double maxRadius;
#ifdef USE_TRANSFORMATIONS
	Shape transformed;
#endif
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
	bool intersects(const Part& other, Vec3& intersection, Vec3& exitVector) const;
};

#include "physical.h"
