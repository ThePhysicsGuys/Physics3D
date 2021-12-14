#pragma once

#include <vector>
#include "constraint.h"

namespace P3D {
class Physical;
class Part;

class PhysicalConstraint {
public:
	Physical* physA;
	Physical* physB;
	Constraint* constraint;

	inline PhysicalConstraint(Physical* physA, Physical* physB, Constraint* constraint) :
		physA(physA), physB(physB), constraint(constraint) {}

	int maxNumberOfParameters() const;
	ConstraintMatrixPack getMatrices(double* matrixBuf, double* errorBuf) const;
};

class ConstraintGroup {
public:
	std::vector<PhysicalConstraint> constraints;
	//std::vector<MotorizedPhysical*> physicals;

	void add(Physical* first, Physical* second, Constraint* constraint);
	void add(Part* first, Part* second, Constraint* constraint);

	void apply() const;
};
}
