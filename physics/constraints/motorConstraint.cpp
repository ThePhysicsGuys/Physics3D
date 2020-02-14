#include "motorConstraint.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "../math/linalg/trigonometry.h"

#include "../../util/log.h"


MotorConstraint::MotorConstraint(NormalizedVec3 direction, double speed, double currentAngle) :
	speed(speed), 
	direction(direction), 
	currentAngle(currentAngle) {}

MotorConstraint::MotorConstraint(Vec3 angularVelocity, double currentAngle) : MotorConstraint(normalize(angularVelocity), length(angularVelocity), currentAngle) {}
MotorConstraint::MotorConstraint(Vec3 angularVelocity) : MotorConstraint(angularVelocity, 0.0) {}

void MotorConstraint::update(double deltaT) {
	this->currentAngle = this->currentAngle + deltaT * speed;
	this->currentAngle = fmod(fmod(this->currentAngle, 2 * M_PI) + 2*M_PI, 2*M_PI);
}

void MotorConstraint::invert() { Log::error("MotorConstraint::invert is not implemented!"); }

CFrame MotorConstraint::getRelativeCFrame() const {
	return CFrame(fromRotationVec(direction * currentAngle));
}

RelativeMotion MotorConstraint::getRelativeMotion() const {
	return RelativeMotion(Motion(Vec3(0, 0, 0), direction * speed), CFrame(fromRotationVec(direction * currentAngle)));
}
