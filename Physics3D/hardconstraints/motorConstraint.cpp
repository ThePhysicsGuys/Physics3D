#include "motorConstraint.h"

#include "../math/linalg/trigonometry.h"
#include "../math/constants.h"

#include "../misc/debug.h"

namespace P3D {
void ConstantMotorTurner::update(double deltaT) {
	this->currentAngle = fmod(fmod(this->currentAngle + deltaT * speed, 2 * PI) + 2 * PI, 2 * PI);
}

void ConstantMotorTurner::invert() { Debug::logError("ConstantSpeedMotorConstraint::invert is not implemented!"); assert(false); }


double ConstantMotorTurner::getValue() const {
	return currentAngle;
}
FullTaylor<double> ConstantMotorTurner::getFullTaylorExpansion() const {
	return FullTaylor<double>::fromConstantAndDerivatives(this->currentAngle, Taylor<double>{speed});
}
};