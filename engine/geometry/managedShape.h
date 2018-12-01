#pragma once

#include "shape.h"


class ManagedShape {
private:
	Shape shape;
	int * vCopyCount;
	int * tCopyCount;

	ManagedShape(Shape s, int* vCopyCount, int* tCopyCount);
public:

	ManagedShape();
	ManagedShape(Shape s);
	ManagedShape(Vec3* vertices, Triangle* triangles, int vertexCount, int triangleCount);
	ManagedShape(const ManagedShape& s);
	ManagedShape(ManagedShape&& s);
	ManagedShape& operator=(const ManagedShape& s);
	ManagedShape& operator=(ManagedShape&& s);

	~ManagedShape();

	ManagedShape translated(Vec3 offset) const;
	ManagedShape rotated(RotMat3 rotation) const;

	inline const Shape& getShape() const { return shape; };
};
