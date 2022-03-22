#include "core.h"
#include "extendedTriangleMesh.h"

#include <Physics3D/geometry/polyhedron.h>

namespace P3D::Graphics {

// Todo move to TriangleMesh
ExtendedTriangleMesh ExtendedTriangleMesh::generateSmoothNormalsShape(const TriangleMesh& underlyingMesh) {
	Vec3f* normalBuffer = new Vec3f[underlyingMesh.vertexCount];
	underlyingMesh.computeNormals(normalBuffer);

	ExtendedTriangleMesh result(underlyingMesh);
	result.setNormalBuffer(SRef<const Vec3f[]>(normalBuffer));

	return result;
}

// Todo move to TriangleMesh
ExtendedTriangleMesh ExtendedTriangleMesh::generateSplitNormalsShape(const TriangleMesh& underlyingMesh) {
	Vec3f* newVertices = new Vec3f[underlyingMesh.triangleCount * 3];
	Vec3f* newNormals = new Vec3f[underlyingMesh.triangleCount * 3];
	Triangle* newTriangles = new Triangle[underlyingMesh.triangleCount];

	for (int i = 0; i < underlyingMesh.triangleCount; i++) {
		Triangle t = underlyingMesh.getTriangle(i);

		Vec3f a = underlyingMesh.getVertex(t.firstIndex);
		Vec3f b = underlyingMesh.getVertex(t.secondIndex);
		Vec3f c = underlyingMesh.getVertex(t.thirdIndex);

		Vec3f normal = normalize(underlyingMesh.getNormalVecOfTriangle(t));

		newVertices[i * 3] = a;
		newVertices[i * 3 + 1] = b;
		newVertices[i * 3 + 2] = c;


		newNormals[i * 3] = normal;
		newNormals[i * 3 + 1] = normal;
		newNormals[i * 3 + 2] = normal;

		newTriangles[i] = Triangle{i * 3, i * 3 + 1, i * 3 + 2};
	}

	ExtendedTriangleMesh result(TriangleMesh(underlyingMesh.triangleCount * 3, underlyingMesh.triangleCount, newVertices, newTriangles));
	result.setNormalBuffer(SRef<const Vec3f[]>(newNormals));

	return result;
}

}
