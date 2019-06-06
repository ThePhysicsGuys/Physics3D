#pragma once

#include "../engine/math/vec2.h"
#include "../engine/math/vec3.h"
#include "../engine/geometry/shape.h"

struct VisualShape : public Shape {
	std::shared_ptr<Vec3f> normals;
	std::shared_ptr<Vec2f> uvs;

	VisualShape() : Shape(), normals(nullptr), uvs(nullptr) {}
	VisualShape(Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount) : VisualShape(vertices, nullptr, nullptr, triangles, vertexCount, triangleCount) {}
	VisualShape(Vec3f* vertices, Vec3f* normals, const Triangle* triangles, int vertexCount, int triangleCount) : VisualShape(vertices, normals, nullptr, triangles, vertexCount, triangleCount) {}
	VisualShape(Vec3f* vertices, Vec2f* uvs, const Triangle* triangles, int vertexCount, int triangleCount) : VisualShape(vertices, nullptr, uvs, triangles, vertexCount, triangleCount) {}
	
	explicit VisualShape(const Shape& shape) : VisualShape(shape, nullptr, nullptr) {}
	VisualShape(const Shape& shape, Vec3f* normals, Vec2f* uvs) : Shape(shape), normals(normals), uvs(uvs) {}

	VisualShape(Vec3f* vertices, Vec3f* normals, Vec2f* uvs, const Triangle* triangles, int vertexCount, int triangleCount) :
		Shape(vertices, triangles, vertexCount, triangleCount), normals(normals), uvs(uvs) {}
};
