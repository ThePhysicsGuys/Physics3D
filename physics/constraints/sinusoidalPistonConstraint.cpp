#include "sinusoidalPistonConstraint.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <assert.h>

SinusoidalPistonConstraint::SinusoidalPistonConstraint(Vec3 pistonDirection, double minLength, double maxLength, double period) :
	pistonDirection(pistonDirection),
	minLength(minLength),
	maxLength(maxLength),
	period(period),
	currentStepInPeriod(0) {

	assert(maxLength >= minLength);
}

void SinusoidalPistonConstraint::update(double deltaT) {
	currentStepInPeriod += deltaT;
	if(currentStepInPeriod >= period) currentStepInPeriod -= period;
}

void SinusoidalPistonConstraint::invert() {
	throw "Not implemented!";
}

CFrame SinusoidalPistonConstraint::getRelativeCFrame() const {
	double multiplier = (maxLength - minLength) / 2;
	double offset = minLength + multiplier;

	double divPeriodToRadians = 2 * M_PI / period;

	double currentAngle = currentStepInPeriod * divPeriodToRadians;

	double length = sin(currentAngle) * multiplier + offset;

	return CFrame(pistonDirection * length);
}

RelativeMotion SinusoidalPistonConstraint::getRelativeMotion() const {
	double multiplier = (maxLength - minLength) / 2;
	double offset = minLength + multiplier;

	double divPeriodToRadians = 2 * M_PI / period;

	double currentAngle = currentStepInPeriod * divPeriodToRadians;

	double length = sin(currentAngle) * multiplier + offset;

	double speed = cos(currentAngle) * multiplier * divPeriodToRadians;

	double accel = -sin(currentAngle) * multiplier * divPeriodToRadians * divPeriodToRadians;

	return RelativeMotion(Motion(pistonDirection * speed, Vec3(), pistonDirection * accel, Vec3()), CFrame(pistonDirection * length));
}
