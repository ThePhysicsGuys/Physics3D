#pragma once

#include "../engine/math/vec.h"
#include "../engine/geometry/shape.h"
#include "../engine/datastructures/sharedArray.h"

struct VisualShape : public Shape {
	SharedArrayPtr<const Vec3f> normals;
	SharedArrayPtr<const Vec2f> uvs;

	VisualShape() : Shape(), normals(nullptr), uvs(nullptr) {}
	VisualShape(Vec3f* vertices, SharedArrayPtr<const Triangle> triangles, int vertexCount, int triangleCount) : VisualShape(vertices, SharedArrayPtr<const Vec3f>(), SharedArrayPtr<const Vec2f>(), triangles, vertexCount, triangleCount) {}
	VisualShape(Vec3f* vertices, SharedArrayPtr<const Vec3f> normals, SharedArrayPtr<const Triangle> triangles, int vertexCount, int triangleCount) : VisualShape(vertices, normals, SharedArrayPtr<const Vec2f>(), triangles, vertexCount, triangleCount) {}
	VisualShape(Vec3f* vertices, SharedArrayPtr<const Vec2f> uvs, SharedArrayPtr<const Triangle> triangles, int vertexCount, int triangleCount) : VisualShape(vertices, SharedArrayPtr<const Vec3f>(), uvs, triangles, vertexCount, triangleCount) {}
	
	explicit VisualShape(const Shape& shape) : VisualShape(shape, SharedArrayPtr<const Vec3f>(), SharedArrayPtr<const Vec2f>()) {}
	VisualShape(const Shape& shape, SharedArrayPtr<const Vec3f> normals, SharedArrayPtr<const Vec2f> uvs) : Shape(shape), normals(normals), uvs(uvs) {}

	VisualShape(Vec3f* vertices, SharedArrayPtr<const Vec3f> normals, SharedArrayPtr<const Vec2f> uvs, SharedArrayPtr<const Triangle> triangles, int vertexCount, int triangleCount) :
		Shape(vertices, triangles, vertexCount, triangleCount), normals(normals), uvs(uvs) {}
};
