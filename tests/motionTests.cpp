#include "testsMain.h"

#include "compare.h"
#include "randomValues.h"
#include "../physics/misc/toString.h"
#include "estimateMotion.h"

#include "../physics/motion.h"
#include "../physics/relativeMotion.h"

#include "../physics/constraints/hardPhysicalConnection.h"

#include "../physics/constraints/motorConstraint.h"
#include "../physics/constraints/sinusoidalPistonConstraint.h"


#define REMAINS_CONSTANT(v) REMAINS_CONSTANT_TOLERANT(v, 0.005)
#define ASSERT(v) ASSERT_TOLERANT(v, 0.005)

#define DELTA_T 0.0001



TEST_CASE(testBasicMotion) {
	Vec3 vel(1.3, 0.7, 0.9);
	Vec3 angularVel(1.7, 0.9, 1.2);
	Vec3 point(1.2, -0.5, 0.7);

	Motion m(vel, angularVel);

	ASSERT_TOLERANT(m.getVelocityOfPoint(point) == getVelocityBySimulation(m, point, DELTA_T), 0.05);
}

TEST_CASE(testAdvancedMotion) {
	Vec3 vel(1.3, 0.7, 0.9);
	Vec3 angularVel(1.7, 0.9, 1.2);
	Vec3 accel(7.1, 0.9, -3.8);
	Vec3 angularAccel(1.8, 0.67, -4.1);
	Vec3 point(1.2, -0.5, 0.7);

	Motion m(vel, angularVel, accel, angularAccel);

	ASSERT_TOLERANT(m.getMotionOfPoint(point) == getMotionBySimulation(m, point, DELTA_T), 0.05);
}

TEST_CASE(testMotionOfMotorConstraintCorrect) {
	HardPhysicalConnection connection(std::unique_ptr<HardConstraint>(new ConstantSpeedMotorConstraint(createRandomDouble())), createRandomCFrame(), createRandomCFrame());

	connection.update(1.23456789);

	RelativeMotion calculatedMotion = connection.getRelativeMotion();

	CFrame cf1 = connection.getRelativeCFrameToParent();
	connection.update(DELTA_T);
	CFrame cf2 = connection.getRelativeCFrameToParent();
	connection.update(DELTA_T);
	CFrame cf3 = connection.getRelativeCFrameToParent();

	ASSERT(calculatedMotion.relativeMotion == estimateMotion(cf1, cf2, cf3, DELTA_T));
}

TEST_CASE(testMotionOfPistonConstraintCorrect){
	HardPhysicalConnection connection(std::unique_ptr<HardConstraint>(new SinusoidalPistonConstraint(1.0, 5.0, 1.0)), createRandomCFrame(), createRandomCFrame());

	connection.update(1.23456789);

	RelativeMotion calculatedMotion = connection.getRelativeMotion();

	CFrame cf1 = connection.getRelativeCFrameToParent();
	connection.update(DELTA_T);
	CFrame cf2 = connection.getRelativeCFrameToParent();
	connection.update(DELTA_T);
	CFrame cf3 = connection.getRelativeCFrameToParent();

	ASSERT(calculatedMotion.relativeMotion == estimateMotion(cf1, cf2, cf3, DELTA_T));
}

TEST_CASE(testPistonConstraint) {
	SinusoidalPistonConstraint constraint(1.0, 2.5, 0.7);

	constraint.update(1.23456789);

	CFrame cf1 = constraint.getRelativeCFrame();

	RelativeMotion relMotion = constraint.getRelativeMotion();

	constraint.update(DELTA_T);

	CFrame cf2 = constraint.getRelativeCFrame();

	ASSERT((cf2.getPosition() - cf1.getPosition()) == relMotion.relativeMotion.getVelocity() * DELTA_T);
}

TEST_CASE(testExtendBegin) {
	RelativeMotion relMotion = createRandomRelativeMotion();
	Vec3 offsetFromBegin = createRandomNonzeroVec3();

	RelativeMotion resultingMotion1 = relMotion.extendBegin(offsetFromBegin);
	RelativeMotion resultingMotion2 = RelativeMotion(Motion(), CFrame(offsetFromBegin)) + relMotion;

	ASSERT(resultingMotion1 == resultingMotion2);
}

TEST_CASE(testExtendEnd) {
	RelativeMotion relMotion = createRandomRelativeMotion();
	Vec3 offsetFromEnd = createRandomNonzeroVec3();

	RelativeMotion resultingMotion1 = relMotion.extendEnd(offsetFromEnd);
	RelativeMotion resultingMotion2 = relMotion + RelativeMotion(Motion(), CFrame(offsetFromEnd));

	ASSERT(resultingMotion1 == resultingMotion2);
}

TEST_CASE(testExtendingRelativeMotionCVecCommutes) {
	RelativeMotion relMotion = createRandomRelativeMotion();
	Vec3 offsetFromBegin = createRandomNonzeroVec3();
	Vec3 offsetFromEnd = createRandomNonzeroVec3();

	RelativeMotion resultingMotion1 = relMotion.extendBegin(offsetFromBegin).extendEnd(offsetFromEnd);
	RelativeMotion resultingMotion2 = relMotion.extendEnd(offsetFromEnd).extendBegin(offsetFromBegin);

	ASSERT(resultingMotion1 == resultingMotion2);
}
TEST_CASE(testExtendingRelativeMotionCFrameCommutes) {
	RelativeMotion relMotion = createRandomRelativeMotion();
	CFrame offsetFromBegin = createRandomCFrame();
	CFrame offsetFromEnd = createRandomCFrame();

	RelativeMotion resultingMotion1 = relMotion.extendBegin(offsetFromBegin).extendEnd(offsetFromEnd);
	RelativeMotion resultingMotion2 = relMotion.extendEnd(offsetFromEnd).extendBegin(offsetFromBegin);

	ASSERT(resultingMotion1 == resultingMotion2);
}
TEST_CASE(testExtendingRelativeMotionVecCorrect){
	RelativeMotion relMotion = createRandomRelativeMotion();
	Motion motionOfOrigin = createRandomMotion();
	Vec3 offsetFromBegin = createRandomNonzeroVec3();
	Vec3 offsetFromEnd = createRandomNonzeroVec3();

	// manually compute resulting motion
	Motion motionOfStartPoint = motionOfOrigin.getMotionOfPoint(offsetFromBegin);
	Motion motionOfEnd = motionOfStartPoint.getMotionOfPoint(relMotion.locationOfRelativeMotion.getPosition()).addRelativeMotion(relMotion.relativeMotion);

	RelativeMotion relMotionOfBeginIncluded = relMotion.extendBegin(offsetFromBegin);
	ASSERT(motionOfEnd == relMotionOfBeginIncluded.applyTo(motionOfOrigin));
}

TEST_CASE(testExtendingRelativeMotionCFrameCorrect) {
	RelativeMotion relMotion = createRandomRelativeMotion();
	Motion motionOfOrigin = createRandomMotion();
	CFrame offsetFromBegin = createRandomCFrame();
	CFrame offsetFromEnd = createRandomCFrame();

	// manually compute resulting motion
	Motion motionOfStartPoint = motionOfOrigin.getMotionOfPoint(offsetFromBegin.getPosition());
	Motion rotatedRelativeMotion = localToGlobal(offsetFromBegin.getRotation(), relMotion.relativeMotion);
	CFrame rotatedOffset = offsetFromBegin.localToRelative(relMotion.locationOfRelativeMotion);
	Motion motionOfEnd = motionOfStartPoint.getMotionOfPoint(rotatedOffset.getPosition()).addRelativeMotion(rotatedRelativeMotion);

	RelativeMotion relMotionOfBeginIncluded = relMotion.extendBegin(offsetFromBegin);
	ASSERT(motionOfEnd == relMotionOfBeginIncluded.applyTo(motionOfOrigin));
}

TEST_CASE(testJoiningRelativeMotionCorrect) {
	RelativeMotion r1 = createRandomRelativeMotion();
	RelativeMotion r2 = createRandomRelativeMotion();

	Motion motionOfEndPoint = r1.relativeMotion;
	CFrame offsetForSecondEndPoint = r1.locationOfRelativeMotion.localToRelative(r2.locationOfRelativeMotion);

	Motion secondMotionInNewSpace = localToGlobal(r1.locationOfRelativeMotion.getRotation(), r2.relativeMotion);

	Motion motionOfSecondEndPoint = 
		motionOfEndPoint.getMotionOfPoint(offsetForSecondEndPoint.getPosition())
		.addRelativeMotion(secondMotionInNewSpace);

	ASSERT((r1 + r2).locationOfRelativeMotion == offsetForSecondEndPoint + r1.locationOfRelativeMotion.getPosition());
	ASSERT((r1 + r2).relativeMotion == motionOfSecondEndPoint);
}

TEST_CASE(testComplexRelativeMotionAssociative) {
	Motion m0 = createRandomMotion();
	RelativeMotion r1 = createRandomRelativeMotion();
	RelativeMotion r2 = createRandomRelativeMotion();
	RelativeMotion r3 = createRandomRelativeMotion();

	ASSERT((r1 + r2) + r3 == r1 + (r2 + r3));
}

TEST_CASE(testReducedRelativeMotionAssociative) {
	Vec3 piston1Direction = createRandomNonzeroVec3();
	Vec3 piston2Direction = createRandomNonzeroVec3();
	Vec3 motor1Direction = createRandomNonzeroVec3();
	Vec3 motor2Direction = createRandomNonzeroVec3();

	RelativeMotion p1(Motion(createRandomNonzeroDouble() * piston1Direction, Vec3(), createRandomNonzeroDouble() * piston1Direction, Vec3()), CFrame(createRandomNonzeroDouble() * piston1Direction));
	RelativeMotion p2(Motion(createRandomNonzeroDouble() * piston2Direction, Vec3(), createRandomNonzeroDouble() * piston2Direction, Vec3()), CFrame(createRandomNonzeroDouble() * piston2Direction));
	RelativeMotion m1(Motion(Vec3(), createRandomNonzeroDouble() * motor1Direction, Vec3(), createRandomNonzeroDouble() * motor1Direction), CFrame(createRandomNonzeroDouble() * motor1Direction));
	RelativeMotion m2(Motion(Vec3(), createRandomNonzeroDouble() * motor2Direction, Vec3(), createRandomNonzeroDouble() * motor2Direction), CFrame(createRandomNonzeroDouble() * motor2Direction));

	ASSERT(p1 + p2 == p2 + p1); // pistons are commutative
	ASSERT((p1 + p2) + m1 == p1 + (p2 + m1));
	ASSERT((m1 + m2) + p1 == m1 + (m2 + p1));
	ASSERT(((m1 + p1) + m2) + p2 == m1 + (p1 + (m2 + p2)));
}

TEST_CASE(testSimulateRelativeMotion) {
	Motion motionOfOrigin = createRandomMotion();
	RelativeMotion relMotion = createRandomRelativeMotion();

	/*motionOfOrigin.acceleration = Vec3(0, 0, 0);
	relMotion.relativeMotion.acceleration = Vec3(0, 0, 0);
	motionOfOrigin.angularAcceleration = Vec3(0, 0, 0);
	relMotion.relativeMotion.angularAcceleration = Vec3(0, 0, 0);
	relMotion.relativeMotion.angularVelocity = Vec3(0, 0, 0);*/
	//relMotion.relativeMotion.velocity = Vec3(0, 0, 0); Coriolis effect!

	CFrame origin1 = CFrame();
	CFrame origin2 = simulateForTime(motionOfOrigin, CFrame(), DELTA_T);
	CFrame origin3 = simulateForTime(motionOfOrigin, CFrame(), 2*DELTA_T);

	CFrame relative1 = relMotion.locationOfRelativeMotion;
	CFrame relative2 = simulateForTime(relMotion.relativeMotion, relMotion.locationOfRelativeMotion, DELTA_T);
	CFrame relative3 = simulateForTime(relMotion.relativeMotion, relMotion.locationOfRelativeMotion, 2*DELTA_T);

	CFrame p1 = origin1.localToGlobal(relative1);
	CFrame p2 = origin2.localToGlobal(relative2);
	CFrame p3 = origin3.localToGlobal(relative3);

	Motion estimatedMotion = estimateMotion(p1, p2, p3, DELTA_T);

	Motion calculatedMotion = relMotion.applyTo(motionOfOrigin);

	ASSERT(estimatedMotion == calculatedMotion);
}

TEST_CASE(testSimulateRelativeToRelativeMotion) {
	RelativeMotion motionOfOrigin = createRandomRelativeMotion();
	RelativeMotion relMotion = createRandomRelativeMotion();

	/*motionOfOrigin.acceleration = Vec3(0, 0, 0);
	relMotion.relativeMotion.acceleration = Vec3(0, 0, 0);
	motionOfOrigin.angularAcceleration = Vec3(0, 0, 0);
	relMotion.relativeMotion.angularAcceleration = Vec3(0, 0, 0);
	relMotion.relativeMotion.angularVelocity = Vec3(0, 0, 0);*/
	//relMotion.relativeMotion.velocity = Vec3(0, 0, 0); Coriolis effect!

	CFrame origin1 = motionOfOrigin.locationOfRelativeMotion;
	CFrame origin2 = simulateForTime(motionOfOrigin.relativeMotion, motionOfOrigin.locationOfRelativeMotion, DELTA_T);
	CFrame origin3 = simulateForTime(motionOfOrigin.relativeMotion, motionOfOrigin.locationOfRelativeMotion, 2 * DELTA_T);

	CFrame relative1 = relMotion.locationOfRelativeMotion;
	CFrame relative2 = simulateForTime(relMotion.relativeMotion, relMotion.locationOfRelativeMotion, DELTA_T);
	CFrame relative3 = simulateForTime(relMotion.relativeMotion, relMotion.locationOfRelativeMotion, 2 * DELTA_T);

	CFrame p1 = origin1.localToGlobal(relative1);
	CFrame p2 = origin2.localToGlobal(relative2);
	CFrame p3 = origin3.localToGlobal(relative3);

	Motion estimatedMotion = estimateMotion(p1, p2, p3, DELTA_T);

	RelativeMotion calculatedMotion = motionOfOrigin + relMotion;
	

	ASSERT(estimatedMotion == calculatedMotion.relativeMotion);
	ASSERT(p1 == calculatedMotion.locationOfRelativeMotion);
}
