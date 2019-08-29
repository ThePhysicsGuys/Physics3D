#pragma once

struct Part;
class Physical;
#include "geometry/shape.h"
#include "math/mat3.h"
#include "math/position.h"
#include "math/globalCFrame.h"
#include "math/bounds.h"

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
	~Part();
	bool intersects(const Part& other, Position& intersection, Vec3& exitVector) const;
	void scale(double scaleX, double scaleY, double scaleZ);

	Bounds getStrictBounds() const;
};
