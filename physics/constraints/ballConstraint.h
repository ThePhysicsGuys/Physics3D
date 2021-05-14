#pragma once

#include "../math/linalg/vec.h"
#include "softConstraint.h"

namespace P3D {
struct BallConstraint : public Constraint {
	Vec3 attachA;
	Vec3 attachB;

	BallConstraint() = default;
	inline BallConstraint(Vec3 attachA, Vec3 attachB) :
		attachA(attachA), attachB(attachB) {}

	virtual int maxNumberOfParameters() const override;
	virtual ConstraintMatrixPack getMatrices(const PhysicalInfo& physA, const PhysicalInfo& physB, double* matrixBuf, double* errorBuf) const override;
};
};
