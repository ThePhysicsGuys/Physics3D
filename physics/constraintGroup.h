#pragma once

#include <vector>
#include "math/linalg/vec.h"

class Physical;

struct BallConstraint {
	inline BallConstraint(Vec3 attachA, Vec3 attachB) :
		attachA(attachA), attachB(attachB) {}
	Vec3 attachA;
	Vec3 attachB;

	virtual void doNothing();
};

struct PhysicalConstraint {
	inline PhysicalConstraint(Physical* physA, Physical* physB, BallConstraint* constraint) :
		physA(physA), physB(physB), constraint(constraint) {}

	BallConstraint* constraint;
	
	Physical* physA;
	Physical* physB;
};


struct ConstraintGroup {
	std::vector<PhysicalConstraint> constraints;

	void add(Physical* first, Physical* second, BallConstraint* constraint);

	void apply() const;
};
