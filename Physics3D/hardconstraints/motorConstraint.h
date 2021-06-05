#pragma once

#include "hardConstraint.h"
#include "constraintTemplates.h"

namespace P3D {
class ConstantMotorTurner {
public:
	double speed;
	double currentAngle;

	inline ConstantMotorTurner(double motorSpeed, double currentAngle) :
		speed(motorSpeed), currentAngle(currentAngle) {}
	inline ConstantMotorTurner(double motorSpeed) :
		speed(motorSpeed), currentAngle(0.0) {}

	void update(double deltaT);
	void invert();
	double getValue() const;
	FullTaylor<double> getFullTaylorExpansion() const;
};

typedef MotorConstraintTemplate<ConstantMotorTurner> ConstantSpeedMotorConstraint;
};