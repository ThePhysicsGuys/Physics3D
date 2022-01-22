#pragma once

#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/rotation.h"
#include "../math/cframe.h"
#include "scalableInertialMatrix.h"

#include "triangleMesh.h"

namespace P3D {
class Polyhedron : public TriangleMesh {
public:
	Polyhedron() : TriangleMesh() {}
	explicit Polyhedron(const TriangleMesh& mesh);
	explicit Polyhedron(TriangleMesh&& mesh) noexcept;
	explicit Polyhedron(const MeshPrototype& mesh);
	explicit Polyhedron(MeshPrototype&& mesh) noexcept;
	Polyhedron(const Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount);

	Polyhedron translated(Vec3f offset) const;
	Polyhedron rotated(Rotationf rotation) const;
	Polyhedron localToGlobal(CFramef frame) const;
	Polyhedron globalToLocal(CFramef frame) const;
	Polyhedron scaled(float scaleX, float scaleY, float scaleZ) const;
	Polyhedron scaled(DiagonalMat3f scale) const;
	Polyhedron translatedAndScaled(Vec3f translation, DiagonalMat3f scale) const;

	bool containsPoint(Vec3f point) const;

	double getVolume() const;
	Vec3 getCenterOfMass() const;
	SymmetricMat3 getInertiaAroundCenterOfMass() const;
	SymmetricMat3 getInertia(const CFrame& reference) const;
	ScalableInertialMatrix getScalableInertia(const CFrame& reference) const;
	ScalableInertialMatrix getScalableInertiaAroundCenterOfMass() const;
};
};