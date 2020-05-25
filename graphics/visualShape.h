#pragma once

#include "../physics/math/linalg/vec.h"
#include "../physics/geometry/shape.h"
#include "../physics/geometry/triangleMesh.h"
#include "../physics/datastructures/sharedArray.h"

class Polyhedron;

namespace Graphics {

struct VisualShape : public TriangleMesh {
	typedef SharedArrayPtr<const Vec3f> SVec3f;
	typedef SharedArrayPtr<const Vec2f> SVec2f;

	SVec3f normals;
	SVec2f uvs;
	SVec3f tangents;
	SVec3f bitangents;

	VisualShape() : TriangleMesh(), normals(nullptr), uvs(nullptr), tangents(nullptr), bitangents(nullptr) {}

	VisualShape(Vec3f* vertices, int vertexCount, const Triangle* triangles, int triangleCount, SVec3f normals = SVec3f(), SVec2f uvs = SVec2f(), SVec3f tangents = SVec3f(), SVec3f bitangents = SVec3f()) :
		TriangleMesh(vertices, triangles, vertexCount, triangleCount), normals(normals), uvs(uvs), tangents(tangents), bitangents(bitangents) {}

	explicit VisualShape(const TriangleMesh& shape, SVec3f normals = SVec3f(), SVec2f uvs = SVec2f(), SVec3f tangents = SVec3f(), SVec3f bitangents = SVec3f()) :
		TriangleMesh(shape), normals(normals), uvs(uvs), tangents(tangents), bitangents(bitangents) {}

	static VisualShape generateSmoothNormalsShape(const Polyhedron& underlyingPoly);
	static VisualShape generateSplitNormalsShape(const TriangleMesh& underlyingMesh);
};

};