#pragma once

struct Part;
struct Physical;
#include "geometry/shape.h"
#include "math/mat3.h"

struct PartPhysicalData {
	CFrame cframe;
	Shape hitbox;
	double maxRadius;
	double friction;
	Part* part;

	//PartPhysicalData(const CFrame& cframe, const VisualShape& hitbox)
};

struct Part {
public:
	Physical* parent = nullptr;
	Shape hitbox;
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
	Vec3 localCenterOfMass;

	Part() = default;
	Part(const Shape& shape, const CFrame& position, double density, double friction);
	bool intersects(const Part& other, Vec3& intersection, Vec3& exitVector) const;
};
