#pragma once

struct BoundingBox;
struct Polyhedron;

#include "../math/linalg/vec.h"

struct BoundingBox {
	double xmin, ymin, zmin;
	double xmax, ymax, zmax;

	inline BoundingBox() = default;
	inline BoundingBox(double x, double y, double z) : xmin(-x/2), xmax(x/2), ymin(-y/2), ymax(y/2), zmin(-z/2), zmax(z/2) {};
	inline BoundingBox(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax) : xmin(xmin), ymin(ymin), zmin(zmin), xmax(xmax), ymax(ymax), zmax(zmax) {};

	bool intersects(BoundingBox other) const;
	bool containsPoint(Vec3 point) const;
	Polyhedron toShape() const;
	inline double getWidth() const { return xmax - xmin; }
	inline double getHeight() const { return ymax - ymin; }
	inline double getDepth() const { return zmax - zmin; }
	inline Vec3 getCenter() const { return Vec3(xmin + xmax, ymin + ymax, zmin + zmax) / 2; }
	inline BoundingBox expanded(double amount) const { return BoundingBox(xmin - amount, ymin - amount, zmin - amount, xmax + amount, ymax + amount, zmax + amount); }
};
