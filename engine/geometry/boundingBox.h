#pragma once

struct BoundingBox;

#include "../math/vec3.h"
#include "shape.h"

struct BoundingBox {
	double xmin, ymin, zmin;
	double xmax, ymax, zmax;

	inline BoundingBox(double x, double y, double z) : xmin(-x/2), xmax(x/2), ymin(-y/2), ymax(y/2), zmin(-z/2), zmax(z/2) {};
	inline BoundingBox(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax) : xmin(xmin), ymin(ymin), zmin(zmin), xmax(xmax), ymax(ymax), zmax(zmax) {};

	bool intersects(BoundingBox other) const;
	bool containsPoint(Vec3 point) const;
	Shape toShape(Vec3f vecBuf[8]) const;
};
