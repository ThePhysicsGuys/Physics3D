#pragma once


#include "../math/linalg/vec.h"
#include "softConstraint.h"

namespace P3D {
struct BarConstraint : public Constraint {
	Vec3 attachA;
	Vec3 attachB;
	double length;

	BarConstraint() = default;
	inline BarConstraint(Vec3 attachA, Vec3 attachB, double length) :
		attachA(attachA), attachB(attachB), length(length) {}

	virtual int maxNumberOfParameters() const override;
	virtual ConstraintMatrixPack getMatrices(const PhysicalInfo& physA, const PhysicalInfo& physB, double* matrixBuf, double* errorBuf) const override;
};
};

