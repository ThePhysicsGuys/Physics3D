#pragma once

#include "hardConstraint.h"

class MotorConstraint : public HardConstraint {
	NormalizedVec3 motorDirection;
	double motorSpeed;
	double currentAngle;
public:
	
	MotorConstraint(Vec3 angularVelocity);

	virtual void update(double deltaT) override;
	virtual void invert() override;
	// this CFrame determines the relative position of the constraint's attach1 and attach2
	virtual CFrame getRelativeCFrame() override;

	virtual Motion getRelativeMotion() override;
};
