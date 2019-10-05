#pragma once

#include "../engine/math/linalg/vec.h"
#include "../engine/geometry/shape.h"
#include "../engine/datastructures/sharedArray.h"

struct VisualShape : public Polyhedron {
	SharedArrayPtr<const Vec3f> normals;
	SharedArrayPtr<const Vec2f> uvs;

	VisualShape() : Polyhedron(), normals(nullptr), uvs(nullptr) {}
	VisualShape(Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount) : VisualShape(vertices, SharedArrayPtr<const Vec3f>(), SharedArrayPtr<const Vec2f>(), triangles, vertexCount, triangleCount) {}
	VisualShape(Vec3f* vertices, SharedArrayPtr<const Vec3f> normals, const Triangle* triangles, int vertexCount, int triangleCount) : VisualShape(vertices, normals, SharedArrayPtr<const Vec2f>(), triangles, vertexCount, triangleCount) {}
	VisualShape(Vec3f* vertices, SharedArrayPtr<const Vec2f> uvs, const Triangle* triangles, int vertexCount, int triangleCount) : VisualShape(vertices, SharedArrayPtr<const Vec3f>(), uvs, triangles, vertexCount, triangleCount) {}
	
	explicit VisualShape(const Polyhedron& shape) : VisualShape(shape, SharedArrayPtr<const Vec3f>(), SharedArrayPtr<const Vec2f>()) {}
	VisualShape(const Polyhedron& shape, SharedArrayPtr<const Vec3f> normals, SharedArrayPtr<const Vec2f> uvs) : Polyhedron(shape), normals(normals), uvs(uvs) {}

	VisualShape(Vec3f* vertices, SharedArrayPtr<const Vec3f> normals, SharedArrayPtr<const Vec2f> uvs, const Triangle* triangles, int vertexCount, int triangleCount) :
		Polyhedron(vertices, triangles, vertexCount, triangleCount), normals(normals), uvs(uvs) {}
};
