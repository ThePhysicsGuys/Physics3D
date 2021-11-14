#pragma once


#include "../math/linalg/vec.h"
#include "constraint.h"

namespace P3D {
struct BarConstraint : public Constraint {
	Vec3 attachA;
	Vec3 attachB;
	double barLength;

	BarConstraint() = default;
	inline BarConstraint(Vec3 attachA, Vec3 attachB, double barLength) :
		attachA(attachA), attachB(attachB), barLength(barLength) {}

	virtual int maxNumberOfParameters() const override;
	virtual ConstraintMatrixPack getMatrices(const PhysicalInfo& physA, const PhysicalInfo& physB, double* matrixBuf, double* errorBuf) const override;
};
};

