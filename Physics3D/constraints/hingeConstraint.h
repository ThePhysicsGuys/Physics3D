#pragma once

#include "../math/linalg/vec.h"
#include "constraint.h"

namespace P3D {
struct HingeConstraint : public Constraint {
	Vec3 attachA;
	Vec3 axisA;
	Vec3 attachB;
	Vec3 axisB;

	HingeConstraint() = default;
	inline HingeConstraint(Vec3 attachA, Vec3 axisA, Vec3 attachB, Vec3 axisB) :
		attachA(attachA), attachB(attachB), axisA(axisA), axisB(axisB) {}

	virtual int maxNumberOfParameters() const override;
	virtual ConstraintMatrixPack getMatrices(const PhysicalInfo& physA, const PhysicalInfo& physB, double* matrixBuf, double* errorBuf) const override;
};
};
