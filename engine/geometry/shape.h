#pragma once

struct Triangle;
struct Shape;

#include "../math/vec3.h"
#include "../math/mat3.h"
#include "../math/cframe.h"
#include "boundingBox.h"

struct Triangle {
	unsigned int firstIndex, secondIndex, thirdIndex;

	bool sharesEdgeWith(Triangle other) const;
	Triangle operator~() const;
	bool operator==(const Triangle& other) const;
	Triangle rightShift() const;
	Triangle leftShift() const;
};

struct Shape {
	const Vec3 * vertices;
	const Triangle * triangles;
	int vCount;
	int tCount;

	Shape();
	Shape(const Vec3 * vertices, const Triangle * triangles, int vertexCount, int triangleCount);

	Shape translated(Vec3 offset, Vec3* newVecBuf) const;
	Shape rotated(RotMat3 rotation, Vec3* newVecBuf) const;
	Shape localToGlobal(CFrame frame, Vec3* newVecBuf) const;
	Shape globalToLocal(CFrame frame, Vec3* newVecBuf) const;

	BoundingBox getBounds() const;

	double getVolume() const;
	Vec3 getCenterOfMass() const;

	bool containsPoint(Vec3 point) const;

	bool isValid() const;
};
