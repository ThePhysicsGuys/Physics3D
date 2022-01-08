#include "estimateMotion.h"

#include <Physics3D/math/linalg/mat.h>
#include <Physics3D/math/linalg/trigonometry.h>

namespace P3D {
Vec3 getVelocityBySimulation(const Motion& m, const Vec3& point, double deltaT) {
	Rotation rotation = Rotation::fromRotationVector(m.getAngularVelocity() * deltaT);
	Vec3 delta = m.getVelocity() * deltaT + (rotation * point - point);
	return delta / deltaT;
}

Motion getMotionBySimulation(const Motion& m, const Vec3& point, double deltaT) {
	Vec3 mainVel1 = m.getVelocity();
	Vec3 mainVel2 = mainVel1 + m.getAcceleration() * deltaT;

	Vec3 angularVel2 = m.getAngularVelocity() + m.getAngularAcceleration() * deltaT;
	Vec3 rotVec1 = m.getAngularVelocity() * deltaT;
	Vec3 rotVec2 = angularVel2 * deltaT;
	Rotation rotation1 = Rotation::fromRotationVector(rotVec1);
	Rotation rotation2 = Rotation::fromRotationVector(rotVec2);

	Vec3 pos0 = point;
	Vec3 pos1 = mainVel1 * deltaT + rotation1 * pos0;
	Vec3 pos2 = mainVel2 * deltaT + rotation2 * pos1;

	Vec3 delta1 = pos1 - pos0;
	Vec3 delta2 = pos2 - pos1;
	Vec3 vel1 = delta1 / deltaT;
	Vec3 vel2 = delta2 / deltaT;

	TranslationalMotion tResult(vel1, (vel2 - vel1) / deltaT);
	RotationalMotion rResult(m.getAngularVelocity(), m.getAngularAcceleration());

	return Motion(tResult, rResult);
}
TranslationalMotion estimateMotion(const Vec3& startPos, const Vec3& midPos, const Vec3& endPos, double stepT) {
	Vec3 distance1 = midPos - startPos;
	Vec3 distance2 = endPos - midPos;

	Vec3 velocity1 = distance1 / stepT;
	Vec3 velocity2 = distance2 / stepT;

	Vec3 acceleration = (velocity2 - velocity1) / stepT;

	return TranslationalMotion(velocity1, acceleration);
}
TranslationalMotion estimateMotion(const Position& startPos, const Position& midPos, const Position& endPos, double stepT) {
	Vec3 distance1 = midPos - startPos;
	Vec3 distance2 = endPos - midPos;

	Vec3 velocity1 = distance1 / stepT;
	Vec3 velocity2 = distance2 / stepT;

	Vec3 acceleration = (velocity2 - velocity1) / stepT;

	return TranslationalMotion(velocity1, acceleration);
}

// secondRot = delta * firstRot
// delta = secondRot * ~firstRot

static Vec3 getRotationVecFor(const Rotation& firstRot, const Rotation& secondRot) {
	Rotation delta = secondRot * ~firstRot;

	return delta.asRotationVector();
}

Vec3 getRotationVelFor(const Rotation& before, const Rotation& after, double deltaT) {
	Vec3 rotationVec = getRotationVecFor(before, after);

	return rotationVec / deltaT;
}

Motion estimateMotion(const CFrame& step1, const CFrame& step2, const CFrame& step3, double stepT) {
	Vec3 vel1 = Vec3(step2.getPosition() - step1.getPosition()) / stepT;
	Vec3 vel2 = Vec3(step3.getPosition() - step2.getPosition()) / stepT;

	Vec3 rotVel1 = getRotationVelFor(step1.getRotation(), step2.getRotation(), stepT);
	Vec3 rotVel2 = getRotationVelFor(step2.getRotation(), step3.getRotation(), stepT);

	Vec3 accel = (vel2 - vel1) / stepT;
	Vec3 rotAccel = (rotVel2 - rotVel1) / stepT;

	return Motion(vel1, rotVel1, accel, rotAccel);
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


CFrame simulateForTime(const Motion& motion, const CFrame& startingCFrame, double deltaT) {
	Movement mov = motion.getMovementAfterDeltaT(deltaT);

	return CFrame(startingCFrame.getPosition() + mov.translation, Rotation::fromRotationVector(mov.rotation) * startingCFrame.getRotation());
}
};
