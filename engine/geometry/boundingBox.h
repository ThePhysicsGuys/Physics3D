#pragma once

struct BoundingBox;

#include "../math/vec3.h"
#include "shape.h"

struct BoundingBox {
	double xmin, ymin, zmin;
	double xmax, ymax, zmax;

	bool intersects(BoundingBox other) const;
	bool containsPoint(Vec3 point) const;
	NormalizedShape toShape(Vec3 vecBuf[8]) const;
};
