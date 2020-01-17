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
	double currentAngle = this->currentAngle + deltaT * speed;
	if(currentAngle < 0) currentAngle += 2 * M_PI;
	if(currentAngle > 2 * M_PI) currentAngle -= 2 * M_PI;
}

void MotorConstraint::invert() { Log::error("MotorConstraint::invert is not implemented!"); }

CFrame MotorConstraint::getRelativeCFrame() {
	return CFrame(fromRotationVec(direction * currentAngle));
}

Motion MotorConstraint::getRelativeMotion() {
	return Motion(Vec3(0,0,0), direction * speed);
}
