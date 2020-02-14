#pragma once

#include "../math/cframe.h"
#include "../motion.h"
#include "../relativeMotion.h"


/*
	A HardConstraint is a constraint that fully defines one object in terms of another
*/
class HardConstraint {
public:
	virtual void update(double deltaT) = 0;
	virtual void invert() = 0;
	virtual RelativeMotion getRelativeMotion() const = 0;
	
	virtual CFrame getRelativeCFrame() const;
	
	virtual ~HardConstraint() {}
};
