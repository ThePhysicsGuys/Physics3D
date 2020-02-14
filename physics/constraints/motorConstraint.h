#pragma once

#include "hardConstraint.h"

class MotorConstraint : public HardConstraint {
	NormalizedVec3 direction;
	double speed;
	double currentAngle;

	MotorConstraint(NormalizedVec3 motorDirection, double motorSpeed, double currentAngle);
public:
	MotorConstraint(Vec3 angularVelocity, double currentAngle);
	MotorConstraint(Vec3 angularVelocity);

	inline const NormalizedVec3& getDirection() const { return direction; }
	inline double getSpeed() const { return speed; }
	inline Vec3 getAngularVelocity() const { return direction * speed; }
	inline double getCurrentAngle() const { return currentAngle; }

	virtual void update(double deltaT) override;
	virtual void invert() override;

	virtual CFrame getRelativeCFrame() const override;
	virtual RelativeMotion getRelativeMotion() const override;
};
