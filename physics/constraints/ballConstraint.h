#pragma once

#include "../math/linalg/vec.h"
#include "softConstraint.h"

struct BallConstraint : public Constraint {
	inline BallConstraint(Vec3 attachA, Vec3 attachB) :
		attachA(attachA), attachB(attachB) {}
	Vec3 attachA;
	Vec3 attachB;

	virtual int maxNumberOfParameters() const override;
	virtual ConstraintMatrixPack getMatrices(const PhysicalInfo& physA, const PhysicalInfo& physB, double* matrixBuf, double* errorBuf) const override;
};

