#include "estimateMotion.h"

#include "../physics/math/linalg/mat.h"
#include "../physics/math/linalg/trigonometry.h"

Vec3 getVelocityBySimulation(const Motion& m, const Vec3& point, double deltaT) {
	Mat3 rotation = fromRotationVec(m.angularVelocity * deltaT);
	Vec3 delta = m.velocity * deltaT + (rotation * point - point);
	return delta / deltaT;
}

Motion getMotionBySimulation(const Motion& m, const Vec3& point, double deltaT) {
	Motion result;

	Vec3 mainVel1 = m.velocity;
	Vec3 mainVel2 = mainVel1 + m.acceleration * deltaT;

	Vec3 angularVel2 = m.angularVelocity + m.angularAcceleration * deltaT;
	Vec3 rotVec1 = m.angularVelocity * deltaT;
	Vec3 rotVec2 = angularVel2 * deltaT;
	Mat3 rotation1 = fromRotationVec(rotVec1);
	Mat3 rotation2 = fromRotationVec(rotVec2);

	Vec3 pos0 = point;
	Vec3 pos1 = mainVel1 * deltaT + rotation1 * pos0;
	Vec3 pos2 = mainVel2 * deltaT + rotation2 * pos1;

	Vec3 delta1 = pos1 - pos0;
	Vec3 delta2 = pos2 - pos1;
	Vec3 vel1 = delta1 / deltaT;
	Vec3 vel2 = delta2 / deltaT;

	result.velocity = vel1;
	result.acceleration = (vel2 - vel1) / deltaT;

	result.angularVelocity = m.angularVelocity;
	result.angularAcceleration = m.angularAcceleration;

	return result;
}
std::pair<Vec3, Vec3> estimateMotion(const Position& startPos, const Position& midPos, const Position& endPos, double stepT) {
	Vec3 distance1 = midPos - startPos;
	Vec3 distance2 = endPos - midPos;

	Vec3 velocity1 = distance1 / stepT;
	Vec3 velocity2 = distance2 / stepT;

	Vec3 acceleration = (velocity2 - velocity1) / stepT;

	return std::make_pair(velocity1, acceleration);
}

// secondRot = delta * firstRot
// delta = secondRot * ~firstRot

static Vec3 getRotationVecFor(const RotMat3& firstRot, const RotMat3& secondRot) {
	RotMat3 delta = secondRot * ~firstRot;

	return fromRotationMatrix(delta);
}

Vec3 getRotationVelFor(const RotMat3& before, const RotMat3& after, double deltaT) {
	Vec3 rotationVec = getRotationVecFor(before, after);

	return rotationVec / deltaT;
}

Motion estimateMotion(const GlobalCFrame& step1, const GlobalCFrame& step2, const GlobalCFrame& step3, double stepT) {
	Vec3 vel1 = Vec3(step2.getPosition() - step1.getPosition()) / stepT;
	Vec3 vel2 = Vec3(step3.getPosition() - step2.getPosition()) / stepT;

	Vec3 rotVel1 = getRotationVelFor(step1.getRotation(), step2.getRotation(), stepT);
	Vec3 rotVel2 = getRotationVelFor(step2.getRotation(), step3.getRotation(), stepT);

	Vec3 accel = (vel2 - vel1) / stepT;
	Vec3 rotAccel = (rotVel2 - rotVel1) / stepT;

	return Motion(vel1, rotVel1, accel, rotAccel);
}
