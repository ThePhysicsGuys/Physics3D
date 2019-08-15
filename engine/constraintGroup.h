#pragma once

#include <vector>
#include "math/vec.h"

class Physical;

struct BallConstraint {
	Vec3 attachA;
	Physical* a;
	Vec3 attachB;
	Physical* b;
};

struct ConstraintGroup {
	std::vector<BallConstraint> ballConstraints;

	void apply() const;
};
