#pragma once

#include "../math/linalg/largeMatrix.h"


#define NUMBER_OF_ERROR_DERIVATIVES 3

class Physical;

struct Constraint {
	virtual int numberOfParameters() const = 0;
	virtual void getMatrices(const Physical& physA, const Physical& physB, UnmanagedVerticalFixedMatrix<double, 6>& parameterToMotionMatrixA, UnmanagedVerticalFixedMatrix<double, 6>& parameterToMotionMatrixB, UnmanagedHorizontalFixedMatrix<double, 6>& motionToEquationMatrixA, UnmanagedHorizontalFixedMatrix<double, 6>& motionToEquationMatrixB, UnmanagedHorizontalFixedMatrix<double, NUMBER_OF_ERROR_DERIVATIVES>& errorValue) const = 0;
};
