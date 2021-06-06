#include "motorConstraint.h"

#include "../math/linalg/trigonometry.h"
#include "../math/constants.h"

#include "../../util/log.h"

namespace P3D {
void ConstantMotorTurner::update(double deltaT) {
	this->currentAngle = fmod(fmod(this->currentAngle + deltaT * speed, 2 * PI) + 2 * PI, 2 * PI);
}

void ConstantMotorTurner::invert() { Log::error("ConstantSpeedMotorConstraint::invert is not implemented!"); }


double ConstantMotorTurner::getValue() const {
	return currentAngle;
}
FullTaylor<double> ConstantMotorTurner::getFullTaylorExpansion() const {
	return FullTaylor<double>::fromConstantAndDerivatives(this->currentAngle, Taylor<double>{speed});
}
};