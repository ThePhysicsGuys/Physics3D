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
FullTaylor<double> ConstantMotorTurner::getFullTaylorExpansion() const {
	return FullTaylor<double>{this->currentAngle, Taylor<double>{speed}};
}
