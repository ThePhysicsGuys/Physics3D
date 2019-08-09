#pragma once

struct Part;
struct Physical;
#include "geometry/shape.h"
#include "math/mat3.h"
#include "math/position.h"
#include "math/globalCFrame.h"

struct PartPhysicalData {
	GlobalCFrame cframe;
	Shape hitbox;
	double maxRadius;
	double friction;
	Part* part;
};

struct Part {
public:
	Physical* parent = nullptr;
	Shape hitbox;
	double maxRadius;
#ifdef USE_TRANSFORMATIONS
	Shape transformed;
#endif
	GlobalCFrame cframe;
	struct {
		double density;
		double friction;
	} properties;

	double mass;
	SymmetricMat3 inertia;
	Vec3 localCenterOfMass;

	BoundingBox localBounds;

	Part() = default;
	Part(const Shape& shape, const GlobalCFrame& position, double density, double friction);
	bool intersects(const Part& other, Position& intersection, Vec3& exitVector) const;
};
