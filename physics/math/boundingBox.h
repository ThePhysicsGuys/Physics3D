#pragma once

#include "linalg/vec.h"

template<typename T>
struct BoundingBoxTemplate {
	Vector<T, 3> min;
	Vector<T, 3> max;

	inline BoundingBoxTemplate() : min(), max() {};
	inline BoundingBoxTemplate(T x, T y, T z) : min(-x / 2, -y / 2, -z / 2), max(x / 2, y / 2, z / 2) {}
	inline BoundingBoxTemplate(T xmin, T ymin, T zmin, T xmax, T ymax, T zmax) : min(xmin, ymin, zmin), max(xmax, ymax, zmax) {};
	inline BoundingBoxTemplate(Vector<T, 3> min, Vector<T, 3> max) : min(min), max(max) {}

	inline bool intersects(const BoundingBoxTemplate& o) const {
		return
			min.x <= o.max.x && max.x >= o.min.x &&
			min.y <= o.max.y && max.y >= o.min.y &&
			min.z <= o.max.z && max.z >= o.min.z;
	}
	inline bool containsPoint(const Vector<T, 3>& point) const {
		return
			point.x >= min.x && point.x <= max.x &&
			point.y >= min.y && point.y <= max.y &&
			point.z >= min.z && point.z <= max.z;
	}

	inline T getWidth() const { return max.x - min.x; }
	inline T getHeight() const { return max.y - min.y; }
	inline T getDepth() const { return max.z - min.z; }
	inline Vector<T, 3> getCenter() const { return (min+max) * 0.5; }
	inline BoundingBoxTemplate expanded(T amount) const { return BoundingBoxTemplate(min.x - amount, min.y - amount, min.z - amount, max.x + amount, max.y + amount, max.z + amount); }
	inline BoundingBoxTemplate scaled(T scaleX, T scaleY, T scaleZ) {
		return BoundingBoxTemplate{min.x * scaleX, min.y * scaleY, min.z * scaleZ, max.x * scaleX, max.y * scaleY, max.z * scaleZ};
	}
};

typedef BoundingBoxTemplate<double> BoundingBox;

struct CircumscribingSphere {
	Vec3 origin = Vec3();
	double radius = 0;
};
