#pragma once

#include "../math/cframe.h"
#include "../motion.h"

/*
	A HardConstraint is a constraint that fully defines one object in terms of another
*/
class HardConstraint {
public:
	virtual void update(double deltaT) = 0;
	virtual void invert() = 0;
	// this CFrame determines the relative position of the constraint's attach1 and attach2
	virtual CFrame getRelativeCFrame() = 0;

	virtual Motion getRelativeMotion() = 0;

	virtual ~HardConstraint() {}
};
