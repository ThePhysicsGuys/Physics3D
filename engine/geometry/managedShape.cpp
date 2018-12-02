#include "managedShape.h"

#include "shape.h"

#include "../../util/Log.h"

ManagedShape::ManagedShape() : shape(), vCopyCount(new int(0)), tCopyCount(new int(0)) {}

ManagedShape::ManagedShape(Shape s) : shape(shape), vCopyCount(new int(0)), tCopyCount(new int(0)) {}

ManagedShape::ManagedShape(Vec3 * vertices, Triangle * triangles, int vertexCount, int triangleCount) : vCopyCount(new int(0)), tCopyCount(new int(0)) {
	Vec3* verts = new Vec3[vertexCount];
	Triangle* triangs = new Triangle[triangleCount];
	
	std::memcpy(verts, vertices, vertexCount * sizeof(Vec3));
	std::memcpy(triangs, triangles, triangleCount * sizeof(Triangle));

	shape = Shape(verts, triangs, vertexCount, triangleCount);
}

ManagedShape::ManagedShape(const ManagedShape & other) : shape(other.shape), vCopyCount(other.vCopyCount), tCopyCount(other.tCopyCount) {
	(*vCopyCount)++;
	(*tCopyCount)++;
}

ManagedShape::ManagedShape(Shape shape, int* vCopyCount, int* tCopyCount) : shape(shape), vCopyCount(vCopyCount), tCopyCount(tCopyCount) {}
ManagedShape::ManagedShape(ManagedShape&& other) : shape(other.shape), vCopyCount(other.vCopyCount), tCopyCount(other.tCopyCount) {}

ManagedShape& ManagedShape::operator=(const ManagedShape& other) {
	shape = other.shape;
	tCopyCount = other.tCopyCount;
	vCopyCount = other.vCopyCount;

	(*vCopyCount)++;
	(*tCopyCount)++;

	return *this;
}

ManagedShape & ManagedShape::operator=(ManagedShape&& other) {
	shape = other.shape;
	tCopyCount = other.tCopyCount;
	vCopyCount = other.vCopyCount;

	return *this;
}

ManagedShape::~ManagedShape() {
	if ((*vCopyCount)-- == 0) {
		delete[] shape.vertices;
		delete vCopyCount;
	}
	if ((*tCopyCount)-- == 0) {
		delete[] shape.triangles;
		delete tCopyCount;
	}
}

ManagedShape ManagedShape::translated(Vec3 offset) const {
	Vec3* newVecBuf = new Vec3[shape.vCount];
	(*tCopyCount)++;
	return ManagedShape(shape.translated(offset, newVecBuf), new int(0), tCopyCount);
}

ManagedShape ManagedShape::rotated(RotMat3 rotation) const {
	Vec3* newVecBuf = new Vec3[shape.vCount];
	(*tCopyCount)++;
	return ManagedShape(shape.rotated(rotation, newVecBuf), new int(0), tCopyCount);
}

ManagedShape ManagedShape::localToGlobal(CFrame frame) const {
	Vec3* newVecBuf = new Vec3[shape.vCount];
	(*tCopyCount)++;
	return ManagedShape(shape.localToGlobal(frame, newVecBuf), new int(0), tCopyCount);
}

ManagedShape ManagedShape::globalToLocal(CFrame frame) const {
	Vec3* newVecBuf = new Vec3[shape.vCount];
	(*tCopyCount)++;
	return ManagedShape(shape.globalToLocal(frame, newVecBuf), new int(0), tCopyCount);
}

