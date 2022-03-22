#pragma once

#include <Physics3D/math/linalg/vec.h>
#include <Physics3D/geometry/triangleMesh.h>
#include <Physics3D/datastructures/sharedArray.h>

#include "ecs/components.h"

class Polyhedron;

namespace P3D::Graphics {

struct ExtendedTriangleMesh : public TriangleMesh {
	SRef<const Vec3f[]> normals;
	SRef<const Vec2f[]> uvs;
	SRef<const Vec3f[]> tangents;
	SRef<const Vec3f[]> bitangents;

	ExtendedTriangleMesh()
		: TriangleMesh()
		, normals(nullptr)
		, uvs(nullptr)
		, tangents(nullptr)
		, bitangents(nullptr) {}

	ExtendedTriangleMesh(const Vec3f* vertices, int vertexCount, const Triangle* triangles, int triangleCount)
		: TriangleMesh(vertexCount, triangleCount, vertices, triangles) {
	}

	explicit ExtendedTriangleMesh(const TriangleMesh& shape) : TriangleMesh(shape) {}

	void setNormalBuffer(const SRef<const Vec3f[]>& normals) {
		this->normals = normals;
	}

	void setUVBuffer(const SRef<const Vec2f[]>& uvs) {
		this->uvs = uvs;
	}

	void setTangentBuffer(const SRef<const Vec3f[]>& tangents) {
		this->tangents = tangents;
	}

	void setBitangentBuffer(const SRef<const Vec3f[]>& bitangents) {
		this->bitangents = bitangents;
	}

	Comp::Mesh::Flags getFlags() const {
		using namespace Comp;
		return Mesh::Flags_None
			| (normals != nullptr ? Mesh::Flags_Normal : Mesh::Flags_None)
			| (uvs != nullptr ? Mesh::Flags_UV : Mesh::Flags_None)
			| (tangents != nullptr ? Mesh::Flags_Tangent : Mesh::Flags_None)
			| (bitangents != nullptr ? Mesh::Flags_Bitangent : Mesh::Flags_None);
	}

	static ExtendedTriangleMesh generateSmoothNormalsShape(const TriangleMesh& underlyingMesh);
	static ExtendedTriangleMesh generateSplitNormalsShape(const TriangleMesh& underlyingMesh);
};

};
