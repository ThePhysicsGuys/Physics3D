#pragma once

#include "../math/linalg/vec.h"
#include "softConstraint.h"

struct BallConstraint : public Constraint {
	inline BallConstraint(Vec3 attachA, Vec3 attachB) :
		attachA(attachA), attachB(attachB) {}
	Vec3 attachA;
	Vec3 attachB;

	virtual int numberOfParameters() const override;
	virtual void getMatrices(const Physical& physA, const Physical& physB, UnmanagedVerticalFixedMatrix<double, 6>& parameterToMotionMatrixA, UnmanagedVerticalFixedMatrix<double, 6>& parameterToMotionMatrixB, UnmanagedHorizontalFixedMatrix<double, 6>& motionToEquationMatrixA, UnmanagedHorizontalFixedMatrix<double, 6>& motionToEquationMatrixB, UnmanagedHorizontalFixedMatrix<double, NUMBER_OF_ERROR_DERIVATIVES>& errorValue) const override;
};

