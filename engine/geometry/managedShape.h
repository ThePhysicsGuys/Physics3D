#pragma once

class ManagedShape;

#include "shape.h"
#include "../math/vec3.h"
#include "../math/mat3.h"
#include "../math/cframe.h"

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
	ManagedShape localToGlobal(CFrame frame) const;
	ManagedShape globalToLocal(CFrame frame) const;

	inline Shape getShape() const { return shape; };
};
