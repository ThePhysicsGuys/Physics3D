#include "motorConstraint.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "../math/linalg/trigonometry.h"

#include "../../util/log.h"

void ConstantMotorTurner::update(double deltaT) {
	this->currentAngle = fmod(fmod(this->currentAngle + deltaT * speed, 2 * M_PI) + 2*M_PI, 2*M_PI);
}

void ConstantMotorTurner::invert() { Log::error("ConstantSpeedMotorConstraint::invert is not implemented!"); }


double ConstantMotorTurner::getValue() const {
	return currentAngle;
}
FullTaylorExpansion<double, double, NUMBER_OF_DERIVATIVES_IN_MOTION> ConstantMotorTurner::getFullTaylorExpansion() const {
	return {this->currentAngle, TaylorExpansion<double, NUMBER_OF_DERIVATIVES_IN_MOTION>{speed}};
}
