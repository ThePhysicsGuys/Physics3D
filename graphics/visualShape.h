#pragma once

#include <Physics3D/math/linalg/vec.h>
#include <Physics3D/geometry/shape.h>
#include <Physics3D/geometry/triangleMesh.h>
#include <Physics3D/datastructures/sharedArray.h>

class Polyhedron;

namespace P3D::Graphics {

struct VisualShape : public TriangleMesh {
	typedef SharedArrayPtr<const Vec3f> SVec3f;
	typedef SharedArrayPtr<const Vec2f> SVec2f;

	SVec3f normals;
	SVec2f uvs;
	SVec3f tangents;
	SVec3f bitangents;

	VisualShape() : TriangleMesh(), normals(nullptr), uvs(nullptr), tangents(nullptr), bitangents(nullptr) {}

	VisualShape(const Vec3f* vertices, int vertexCount, const Triangle* triangles, int triangleCount, SVec3f normals = SVec3f(), SVec2f uvs = SVec2f(), SVec3f tangents = SVec3f(), SVec3f bitangents = SVec3f()) :
		TriangleMesh(vertexCount, triangleCount, vertices, triangles), normals(normals), uvs(uvs), tangents(tangents), bitangents(bitangents) {}

	explicit VisualShape(const TriangleMesh& shape, SVec3f normals = SVec3f(), SVec2f uvs = SVec2f(), SVec3f tangents = SVec3f(), SVec3f bitangents = SVec3f()) :
		TriangleMesh(shape), normals(normals), uvs(uvs), tangents(tangents), bitangents(bitangents) {}

	static VisualShape generateSmoothNormalsShape(const Polyhedron& underlyingMesh);
	static VisualShape generateSplitNormalsShape(const TriangleMesh& underlyingMesh);
};

};