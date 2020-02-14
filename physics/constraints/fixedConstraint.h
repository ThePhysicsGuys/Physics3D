#pragma once

#include "hardConstraint.h"

// Mostly used for debugging. You should instead use plain part attachments. 
// This constraint should behave exactly the same as a plain part attachment, but being a HardConstraint, it is less performant. 
class FixedConstraint : public HardConstraint {
public:

	virtual void update(double deltaT) override;
	virtual void invert() override;
	
	virtual CFrame getRelativeCFrame() const override;
	virtual RelativeMotion getRelativeMotion() const override;
};
