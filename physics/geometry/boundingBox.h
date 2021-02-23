#pragma once

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

	BoundingBox() : min(), max() {}
	BoundingBox(double x, double y, double z) : xmin(-x/2), ymin(-y/2), zmin(-z / 2), xmax(x / 2), ymax(y/2), zmax(z/2) {}
	BoundingBox(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax) : xmin(xmin), ymin(ymin), zmin(zmin), xmax(xmax), ymax(ymax), zmax(zmax) {}
	BoundingBox(const Vec3& min, const Vec3& max) : min(min), max(max) {}

	[[nodiscard]] bool intersects(const BoundingBox& other) const {
		return
			xmin <= other.xmax && xmax >= other.xmin &&
			ymin <= other.ymax && ymax >= other.ymin &&
			zmin <= other.zmax && zmax >= other.zmin;
	}
	[[nodiscard]] bool containsPoint(const Vec3& point) const {
		return
			point.x >= xmin && point.x <= xmax &&
			point.y >= ymin && point.y <= ymax &&
			point.z >= zmin && point.z <= zmax;
	}

	[[nodiscard]] double getWidth() const { return xmax - xmin; }
	[[nodiscard]] double getHeight() const { return ymax - ymin; }
	[[nodiscard]] double getDepth() const { return zmax - zmin; }
	
	[[nodiscard]] Vec3 getCenter() const { return (min + max) * 0.5; }
	
	[[nodiscard]] BoundingBox expanded(double amount) const { return BoundingBox(xmin - amount, ymin - amount, zmin - amount, xmax + amount, ymax + amount, zmax + amount); }
	
	[[nodiscard]] BoundingBox expanded(const Vec3& point) const { return BoundingBox(
			point.x < xmin ? point.x : xmin,
			point.y < ymin ? point.y : ymin,
			point.z < zmin ? point.z : zmin,
			point.x > xmax ? point.x : xmax,
			point.y > ymax ? point.y : ymax,
			point.z > zmax ? point.z : zmax
	); }
	
	[[nodiscard]] BoundingBox scaled(double scaleX, double scaleY, double scaleZ) {
		return BoundingBox{ xmin * scaleX, ymin * scaleY, zmin * scaleZ, xmax * scaleX, ymax * scaleY, zmax * scaleZ };
	}
};

struct CircumscribingSphere {
	Vec3 origin = Vec3();
	double radius = 0;
};
