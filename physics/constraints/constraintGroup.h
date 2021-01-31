#pragma once

#include <vector>
#include "../math/linalg/vec.h"
#include "softConstraint.h"

class Physical;

struct PhysicalConstraint {
	inline PhysicalConstraint(Physical* physA, Physical* physB, Constraint* constraint) :
		physA(physA), physB(physB), constraint(constraint) {}

	Constraint* constraint;
	
	Physical* physA;
	Physical* physB;

	void getMatrices(UnmanagedVerticalFixedMatrix<double, 6>& parameterToMotionMatrixA, UnmanagedVerticalFixedMatrix<double, 6> &parameterToMotionMatrixB, UnmanagedHorizontalFixedMatrix<double, 6>& motionToEquationMatrixA, UnmanagedHorizontalFixedMatrix<double, 6>& motionToEquationMatrixB, UnmanagedHorizontalFixedMatrix<double, NUMBER_OF_ERROR_DERIVATIVES>& errorValue) const;

};

struct ConstraintGroup {
	std::vector<PhysicalConstraint> constraints;

	void add(Physical* first, Physical* second, Constraint* constraint);
	
	void apply() const;
};
