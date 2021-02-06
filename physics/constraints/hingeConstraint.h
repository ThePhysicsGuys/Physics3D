#pragma once

#include "../math/linalg/vec.h"
#include "softConstraint.h"

struct HingeConstraint : public Constraint {
	inline HingeConstraint(Vec3 attachA, Vec3 axisA, Vec3 attachB, Vec3 axisB) :
		attachA(attachA), attachB(attachB), axisA(axisA), axisB(axisB) {}
	Vec3 attachA;
	Vec3 axisA;
	Vec3 attachB;
	Vec3 axisB;

	virtual int maxNumberOfParameters() const override;
	virtual ConstraintMatrixPack getMatrices(const PhysicalInfo& physA, const PhysicalInfo& physB, double* matrixBuf, double* errorBuf) const override;
};
