#include "visualShape.h"

namespace Graphics {
VisualShape VisualShape::generateSmoothNormalsShape(const Polyhedron& underlyingPoly) {
	Vec3f* normalBuffer = new Vec3f[underlyingPoly.vertexCount];
	underlyingPoly.computeNormals(normalBuffer);

	return VisualShape(underlyingPoly, SharedArrayPtr<const Vec3f>(normalBuffer));
}
VisualShape VisualShape::generateSplitNormalsShape(const Polyhedron& underlyingPoly) {
	Vec3f* newVertices = new Vec3f[underlyingPoly.triangleCount * 3];
	Vec3f* newNormals = new Vec3f[underlyingPoly.triangleCount * 3];
	Triangle* newTriangles = new Triangle[underlyingPoly.triangleCount];

	for(int i = 0; i < underlyingPoly.triangleCount; i++) {
		Triangle t = underlyingPoly.getTriangle(i);

		Vec3f a = underlyingPoly[t.firstIndex];
		Vec3f b = underlyingPoly[t.secondIndex];
		Vec3f c = underlyingPoly[t.thirdIndex];

		Vec3f normal = normalize((b - a) % (c - a));

		newVertices[i*3]     = a;
		newVertices[i*3 + 1] = b;
		newVertices[i*3 + 2] = c;


		newNormals[i*3] = normal;
		newNormals[i*3 + 1] = normal;
		newNormals[i*3 + 2] = normal;

		newTriangles[i] = Triangle{i*3, i*3 + 1, i*3 + 2};
	}

	return VisualShape(Polyhedron(newVertices, newTriangles, underlyingPoly.triangleCount * 3, underlyingPoly.triangleCount), SharedArrayPtr<const Vec3f>(newNormals));
}
}