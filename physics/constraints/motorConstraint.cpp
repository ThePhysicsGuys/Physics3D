#include "motorConstraint.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "../math/linalg/trigonometry.h"

#include "../../util/log.h"

MotorConstraint::MotorConstraint(Vec3 angularVelocity) : motorSpeed(length(angularVelocity)), motorDirection(normalize(angularVelocity)), currentAngle(0.0) {

}

void MotorConstraint::update(double deltaT) {
	double currentAngle = this->currentAngle + deltaT * motorSpeed;
	if(currentAngle < 0) currentAngle += 2 * M_PI;
	if(currentAngle > 2 * M_PI) currentAngle -= 2 * M_PI;
}

void MotorConstraint::invert() { Log::error("MotorConstraint::invert is not implemented!"); }

CFrame MotorConstraint::getRelativeCFrame() {
	return CFrame(fromRotationVec(motorDirection * currentAngle));
}

Motion MotorConstraint::getRelativeMotion() {
	return Motion(Vec3(0,0,0), motorDirection * motorSpeed);
}
