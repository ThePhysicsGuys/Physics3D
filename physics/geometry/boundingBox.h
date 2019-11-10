#pragma once

struct BoundingBox;
struct Polyhedron;

#include "../math/linalg/vec.h"

struct BoundingBox {
	union {
		struct { double xmin, ymin, zmin; };
		Vec3 min;
	};
	union {
		struct { double xmax, ymax, zmax; };
		Vec3 max;
	};

	inline BoundingBox() : min(), max() {};
	inline BoundingBox(double x, double y, double z) : xmin(-x/2), xmax(x/2), ymin(-y/2), ymax(y/2), zmin(-z/2), zmax(z/2) {};
	inline BoundingBox(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax) : xmin(xmin), ymin(ymin), zmin(zmin), xmax(xmax), ymax(ymax), zmax(zmax) {};
	inline BoundingBox(Vec3 min, Vec3 max) : min(min), max(max) {}

	bool intersects(BoundingBox other) const;
	bool containsPoint(Vec3 point) const;
	Polyhedron toShape() const;
	inline double getWidth() const { return xmax - xmin; }
	inline double getHeight() const { return ymax - ymin; }
	inline double getDepth() const { return zmax - zmin; }
	inline Vec3 getCenter() const { return (min+max) / 2; }
	inline BoundingBox expanded(double amount) const { return BoundingBox(xmin - amount, ymin - amount, zmin - amount, xmax + amount, ymax + amount, zmax + amount); }
	inline BoundingBox scaled(double scaleX, double scaleY, double scaleZ) {
		return BoundingBox{xmin * scaleX, ymin * scaleY, zmin * scaleZ, xmax * scaleX, ymax * scaleY, zmax * scaleZ};
	}
};

struct CircumscribingSphere {
	Vec3 origin = Vec3();
	double radius = 0;
};
