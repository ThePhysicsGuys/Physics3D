#pragma once

struct Part;
struct Physical;
#include "geometry/shape.h"
#include "math/mat3.h"

struct Part {
public:
	Physical* parent = nullptr;
	NormalizedShape hitbox;
	double maxRadius;
#ifdef USE_TRANSFORMATIONS
	Shape transformed;
#endif
	CFrame cframe;
	struct {
		double density;
		double friction;
	} properties;

	double mass;
	SymmetricMat3 inertia;

	Part() = default;
	Part(NormalizedShape shape, CFrame position, double density, double friction);
	Part(Shape shape, CFrame position, double density, double friction);
	bool intersects(const Part& other, Vec3& intersection, Vec3& exitVector) const;
	
};
