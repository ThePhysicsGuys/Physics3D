#include "shape.h"

#include <stdlib.h>
#include <cstring>

#include "../../util/Log.h"

Shape::Shape() : vertices(nullptr), triangles(nullptr), vCount(0), tCount(0) {}

Shape::Shape(Vec3 * vertices, Triangle * triangles, int vCount, int tCount) : vertices(vertices), triangles(triangles), vCount(vCount), tCount(tCount) {}

Shape Shape::translated(Vec3 offset, Vec3 * newVecBuf) const {
	for (int i = 0; i < this->vCount; i++) {
		newVecBuf[i] = offset + vertices[i];
	}

	return Shape(newVecBuf, triangles, vCount, tCount);
}

Shape Shape::rotated(RotMat3 rotation, Vec3 * newVecBuf) const {
	for (int i = 0; i < this->vCount; i++) {
		newVecBuf[i] = rotation * vertices[i];
	}

	return Shape(newVecBuf, triangles, vCount, tCount);
}
