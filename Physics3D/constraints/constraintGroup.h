#pragma once

#include <vector>
#include "../math/linalg/vec.h"
#include "softConstraint.h"

namespace P3D {
class Physical;
class Part;

class PhysicalConstraint {
public:
	inline PhysicalConstraint(Physical* physA, Physical* physB, Constraint* constraint) :
		physA(physA), physB(physB), constraint(constraint) {}

	Constraint* constraint;

	Physical* physA;
	Physical* physB;

	int maxNumberOfParameters() const;
	ConstraintMatrixPack getMatrices(double* matrixBuf, double* errorBuf) const;
};

class ConstraintGroup {
public:
	std::vector<PhysicalConstraint> constraints; 

	void add(Physical* first, Physical* second, Constraint* constraint);
	void add(Part* first, Part* second, Constraint* constraint);

	void apply() const;
};
};
