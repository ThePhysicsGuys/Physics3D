#pragma once

#include "../math/cframe.h"
#include "../motion.h"
#include "../relativeMotion.h"


/*
	A HardConstraint is a constraint that fully defines one object in terms of another
*/
namespace P3D {
class HardConstraint {
public:
	bool isInverted = false;
	virtual void update(double deltaT) = 0;
	virtual void invert() { isInverted = !isInverted; }
	virtual RelativeMotion getRelativeMotion() const = 0;

	virtual CFrame getRelativeCFrame() const = 0;

	virtual ~HardConstraint();
};
};