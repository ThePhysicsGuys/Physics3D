#pragma once

#include "hardConstraint.h"

class MotorConstraint : public HardConstraint {
public:
	Vec3 motorVelocity;

	virtual void update(double deltaT) override;
	// this CFrame determines the relative position of the constraint's attach1 and attach2
	virtual CFrame getRelativeCFrame() override;

	virtual Motion getRelativeMotion() override;
};
