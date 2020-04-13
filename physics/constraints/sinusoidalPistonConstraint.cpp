#include "sinusoidalPistonConstraint.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <assert.h>

#include "pistonConstraintTemplate.h"

#include "../math/predefinedTaylorExpansions.h"

PistonSineGenerator::PistonSineGenerator(double minLength, double maxLength, double period) :
	minLength(minLength),
	maxLength(maxLength),
	period(period),
	currentStepInPeriod(0) {

	assert(maxLength >= minLength);
}

void PistonSineGenerator::update(double deltaT) {
	currentStepInPeriod += deltaT;
	if(currentStepInPeriod >= period) currentStepInPeriod -= period;
}

void PistonSineGenerator::invert() {
	throw "Not implemented!";
}

double PistonSineGenerator::getValue() const {
	double multiplier = (maxLength - minLength) / 2;
	double offset = minLength + multiplier;

	double currentAngle = currentStepInPeriod * (2 * M_PI / period);

	double length = sin(currentAngle) * multiplier + offset;

	return length;
}

FullTaylorExpansion<double, double, NUMBER_OF_DERIVATIVES_IN_MOTION> PistonSineGenerator::getFullTaylorExpansion() const {
	double multiplier = (maxLength - minLength) / 2;
	double offset = minLength + multiplier;

	double currentAngle = currentStepInPeriod * (2 * M_PI / period);

	return generateFullTaylorForSinWave<double, NUMBER_OF_DERIVATIVES_IN_MOTION>(currentAngle, multiplier);
}

/*CFrame SinusoidalPistonConstraint::getRelativeCFrame() const {
	double multiplier = (maxLength - minLength) / 2;
	double offset = minLength + multiplier;

	double divPeriodToRadians = 2 * M_PI / period;

	double currentAngle = currentStepInPeriod * divPeriodToRadians;

	double length = sin(currentAngle) * multiplier + offset;

	return CFrame(0.0, 0.0, length);
}

RelativeMotion SinusoidalPistonConstraint::getRelativeMotion() const {
	double multiplier = (maxLength - minLength) / 2;
	double offset = minLength + multiplier;

	double divPeriodToRadians = 2 * M_PI / period;

	double currentAngle = currentStepInPeriod * divPeriodToRadians;

	double length = sin(currentAngle) * multiplier + offset;

	double speed = cos(currentAngle) * multiplier * divPeriodToRadians;

	double accel = -sin(currentAngle) * multiplier * divPeriodToRadians * divPeriodToRadians;

	

	return RelativeMotion(Motion(Vec3(0.0, 0.0, speed), Vec3(), Vec3(0.0, 0.0, accel), Vec3()), CFrame(0.0, 0.0, length));
}*/
