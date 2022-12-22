#include "testsMain.h"

#include "compare.h"
#include "generators.h"
#include "estimateMotion.h"
#include <Physics3D/misc/toString.h>

#include <Physics3D/geometry/shape.h>
#include <Physics3D/geometry/shapeCreation.h>
#include <Physics3D/part.h>
#include <Physics3D/physical.h>
#include <Physics3D/layer.h>
#include <Physics3D/world.h>
#include <Physics3D/hardconstraints/fixedConstraint.h>
#include <Physics3D/hardconstraints/motorConstraint.h>
#include <Physics3D/hardconstraints/sinusoidalPistonConstraint.h>
#include <Physics3D/math/linalg/trigonometry.h>

#include <functional>

using namespace P3D;
#define ASSERT(cond) ASSERT_TOLERANT(cond, 0.05)

#define DELTA_T 0.0001

TEST_CASE(testMotionOfPhysicalSinglePart) {
	Part p1(sphereShape(1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});

	Physical* p1Phys = p1.ensureHasPhysical();

	Motion COMMotion(Vec3(1.0, 0.7, 1.3), Vec3(-0.3, 1.7, -1.1));

	p1Phys->mainPhysical->motionOfCenterOfMass = COMMotion;

	Vec3 p1calculatedVelBefore = p1.getMotion().getVelocity();

	Vec3 p1calculatedAccelBefore = p1.getMotion().getAcceleration();

	Position p1PosBefore = p1.getCenterOfMass();

	p1Phys->mainPhysical->update(DELTA_T);

	Position p1PosMid = p1.getCenterOfMass();

	p1Phys->mainPhysical->update(DELTA_T);

	Position p1PosAfter = p1.getCenterOfMass();

	TranslationalMotion estimatedVelAccel1 = estimateMotion(p1PosBefore, p1PosMid, p1PosAfter, DELTA_T);

	ASSERT(estimatedVelAccel1.getVelocity() == p1calculatedVelBefore);
	ASSERT(estimatedVelAccel1.getAcceleration() == p1calculatedAccelBefore);
}

TEST_CASE(testMotionOfPhysicalPartsBasic) {
	Part p1(sphereShape(1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part p2(sphereShape(1.0), GlobalCFrame(1.0, 0.0, 0.0), {3.0, 1.0, 1.0});
	p1.attach(&p2, CFrame(1.0, 0.0, 0.0));

	Motion COMMotion(Vec3(1.0, 0.7, 1.3), Vec3(0, 0, 0));

	Physical* p1Phys = p1.getPhysical();
	p1.getMainPhysical()->motionOfCenterOfMass = COMMotion;

	Vec3 p1calculatedVelBefore = p1.getMotion().getVelocity();
	Vec3 p2calculatedVelBefore = p2.getMotion().getVelocity();

	Vec3 p1calculatedAccelBefore = p1.getMotion().getAcceleration();
	Vec3 p2calculatedAccelBefore = p2.getMotion().getAcceleration();

	Position p1PosBefore = p1.getCenterOfMass();
	Position p2PosBefore = p2.getCenterOfMass();

	p1.getMainPhysical()->update(DELTA_T);

	Position p1PosMid = p1.getCenterOfMass();
	Position p2PosMid = p2.getCenterOfMass();

	p1.getMainPhysical()->update(DELTA_T);

	Position p1PosAfter = p1.getCenterOfMass();
	Position p2PosAfter = p2.getCenterOfMass();

	TranslationalMotion estimatedVelAccel1 = estimateMotion(p1PosBefore, p1PosMid, p1PosAfter, DELTA_T);
	TranslationalMotion estimatedVelAccel2 = estimateMotion(p2PosBefore, p2PosMid, p2PosAfter, DELTA_T);

	ASSERT(estimatedVelAccel1.getVelocity() == p1calculatedVelBefore);
	ASSERT(estimatedVelAccel2.getVelocity() == p2calculatedVelBefore);
	ASSERT(estimatedVelAccel1.getAcceleration() == p1calculatedAccelBefore);
	ASSERT(estimatedVelAccel2.getAcceleration() == p2calculatedAccelBefore);
}

TEST_CASE(testMotionOfPhysicalPartsRotation) {
	Part p1(sphereShape(1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part p2(sphereShape(1.0), GlobalCFrame(1.0, 0.0, 0.0), {3.0, 1.0, 1.0});
	p1.attach(&p2, CFrame(1.0, 0.0, 0.0));

	Motion COMMotion(Vec3(0, 0, 0), Vec3(-0.3, 1.7, -1.1));

	p1.getMainPhysical()->motionOfCenterOfMass = COMMotion;

	Vec3 p1calculatedVelBefore = p1.getMotion().getVelocity();
	Vec3 p2calculatedVelBefore = p2.getMotion().getVelocity();

	logStream << p1.getPhysical()->getMotion() << "\n";
	logStream << p1.getMotion() << "\n";
	logStream << p2.getMotion() << "\n";

	Vec3 p1calculatedAccelBefore = p1.getMotion().getAcceleration();
	Vec3 p2calculatedAccelBefore = p2.getMotion().getAcceleration();

	GlobalCFrame p1CFrameBefore = p1.getCFrame();
	GlobalCFrame p2CFrameBefore = p2.getCFrame();

	p1.getMainPhysical()->update(DELTA_T);

	GlobalCFrame p1CFrameMid = p1.getCFrame();
	GlobalCFrame p2CFrameMid = p2.getCFrame();

	p1.getMainPhysical()->update(DELTA_T);

	GlobalCFrame p1CFrameAfter = p1.getCFrame();
	GlobalCFrame p2CFrameAfter = p2.getCFrame();

	Motion estimatedMotion1 = estimateMotion(p1CFrameBefore, p1CFrameMid, p1CFrameAfter, DELTA_T);
	Motion estimatedMotion2 = estimateMotion(p2CFrameBefore, p2CFrameMid, p2CFrameAfter, DELTA_T);

	ASSERT(estimatedMotion1.getVelocity() == p1calculatedVelBefore);
	ASSERT(estimatedMotion2.getVelocity() == p2calculatedVelBefore);
	//ASSERT(estimatedMotion1.getAcceleration() == p1calculatedAccelBefore);
	//ASSERT(estimatedMotion2.getAcceleration() == p2calculatedAccelBefore);
}

TEST_CASE(testMotionOfPhysicalPartsBasicFixedConstraint) {
	Part p1(sphereShape(1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part p2(sphereShape(1.0), GlobalCFrame(1.0, 0.0, 0.0), {3.0, 1.0, 1.0});

	p1.attach(&p2, new FixedConstraint(), CFrame(1.0, 0.0, 0.0), CFrame(0, 0, 0));

	Motion COMMotion(Vec3(1.0, 0.7, 1.3), Vec3(0, 0, 0));

	p1.getMainPhysical()->motionOfCenterOfMass = COMMotion;

	Vec3 p1calculatedVelBefore = p1.getMotion().getVelocity();
	Vec3 p2calculatedVelBefore = p2.getMotion().getVelocity();

	Vec3 p1calculatedAccelBefore = p1.getMotion().getAcceleration();
	Vec3 p2calculatedAccelBefore = p2.getMotion().getAcceleration();

	Position p1PosBefore = p1.getCenterOfMass();
	Position p2PosBefore = p2.getCenterOfMass();

	p1.getMainPhysical()->update(DELTA_T);

	Position p1PosMid = p1.getCenterOfMass();
	Position p2PosMid = p2.getCenterOfMass();

	p1.getMainPhysical()->update(DELTA_T);

	Position p1PosAfter = p1.getCenterOfMass();
	Position p2PosAfter = p2.getCenterOfMass();

	TranslationalMotion estimatedVelAccel1 = estimateMotion(p1PosBefore, p1PosMid, p1PosAfter, DELTA_T);
	TranslationalMotion estimatedVelAccel2 = estimateMotion(p2PosBefore, p2PosMid, p2PosAfter, DELTA_T);

	ASSERT(estimatedVelAccel1.getVelocity() == p1calculatedVelBefore);
	ASSERT(estimatedVelAccel2.getVelocity() == p2calculatedVelBefore);
	ASSERT(estimatedVelAccel1.getAcceleration() == p1calculatedAccelBefore);
	ASSERT(estimatedVelAccel2.getAcceleration() == p2calculatedAccelBefore);
}

TEST_CASE(testMotionOfPhysicalPartsRotationFixedConstraint) {
	Part p1(sphereShape(1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part p2(sphereShape(1.0), GlobalCFrame(1.0, 0.0, 0.0), {3.0, 1.0, 1.0});

	p1.attach(&p2, new FixedConstraint(), CFrame(1.0, 0.0, 0.0), CFrame(0, 0, 0));

	Motion COMMotion(Vec3(0, 0, 0), Vec3(-0.3, 1.7, -1.1));

	p1.getMainPhysical()->motionOfCenterOfMass = COMMotion;

	Vec3 p1calculatedVelBefore = p1.getMotion().getVelocity();
	Vec3 p2calculatedVelBefore = p2.getMotion().getVelocity();

	logStream << p1.getPhysical()->getMotion() << "\n";
	logStream << p1.getMotion() << "\n";
	logStream << p2.getMotion() << "\n";

	Vec3 p1calculatedAccelBefore = p1.getMotion().getAcceleration();
	Vec3 p2calculatedAccelBefore = p2.getMotion().getAcceleration();

	Position p1PosBefore = p1.getCenterOfMass();
	Position p2PosBefore = p2.getCenterOfMass();

	p1.getMainPhysical()->update(DELTA_T);

	Position p1PosMid = p1.getCenterOfMass();
	Position p2PosMid = p2.getCenterOfMass();

	p1.getMainPhysical()->update(DELTA_T);

	Position p1PosAfter = p1.getCenterOfMass();
	Position p2PosAfter = p2.getCenterOfMass();

	TranslationalMotion estimatedVelAccel1 = estimateMotion(p1PosBefore, p1PosMid, p1PosAfter, DELTA_T);
	TranslationalMotion estimatedVelAccel2 = estimateMotion(p2PosBefore, p2PosMid, p2PosAfter, DELTA_T);

	ASSERT(estimatedVelAccel2.getVelocity() == p2calculatedVelBefore);
	ASSERT(estimatedVelAccel1.getVelocity() == p1calculatedVelBefore);
	//ASSERT(estimatedVelAccel2.getAcceleration() == p2calculatedAccelBefore);
	//ASSERT(estimatedVelAccel1.getAcceleration() == p1calculatedAccelBefore);
}

TEST_CASE(testMotionOfPhysicalParts) {
	Part p1(sphereShape(1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part p2(sphereShape(1.0), GlobalCFrame(1.0, 0.0, 0.0), {3.0, 1.0, 1.0});
	p1.attach(&p2, CFrame(1.0, 0.0, 0.0));

	Motion COMMotion(Vec3(1.0, 0.7, 1.3), Vec3(-0.3, 1.7, -1.1));

	p1.getMainPhysical()->motionOfCenterOfMass = COMMotion;

	Vec3 p1calculatedVelBefore = p1.getMotion().getVelocity();
	Vec3 p2calculatedVelBefore = p2.getMotion().getVelocity();

	Vec3 p1calculatedAccelBefore = p1.getMotion().getAcceleration();
	Vec3 p2calculatedAccelBefore = p2.getMotion().getAcceleration();

	Position p1PosBefore = p1.getCenterOfMass();
	Position p2PosBefore = p2.getCenterOfMass();

	p1.getMainPhysical()->update(DELTA_T);

	Position p1PosMid = p1.getCenterOfMass();
	Position p2PosMid = p2.getCenterOfMass();

	p1.getMainPhysical()->update(DELTA_T);

	Position p1PosAfter = p1.getCenterOfMass();
	Position p2PosAfter = p2.getCenterOfMass();

	TranslationalMotion estimatedVelAccel1 = estimateMotion(p1PosBefore, p1PosMid, p1PosAfter, DELTA_T);
	TranslationalMotion estimatedVelAccel2 = estimateMotion(p2PosBefore, p2PosMid, p2PosAfter, DELTA_T);

	ASSERT(estimatedVelAccel1.getVelocity() == p1calculatedVelBefore);
	ASSERT(estimatedVelAccel2.getVelocity() == p2calculatedVelBefore);
	//ASSERT(estimatedVelAccel1.getAcceleration() == p1calculatedAccelBefore);
	//ASSERT(estimatedVelAccel2.getAcceleration() == p2calculatedAccelBefore);
}

TEST_CASE(testMotionOfPhysicalJointsBasic) {
	Part p1(sphereShape(1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part p2(sphereShape(1.0), GlobalCFrame(1.0, 0.0, 0.0), {3.0, 1.0, 1.0});
	p1.attach(&p2, CFrame(1.0, 0.0, 0.0));

	Part p1e(sphereShape(1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part p2e(sphereShape(1.0), GlobalCFrame(1.0, 0.0, 0.0), {3.0, 1.0, 1.0});

	p1e.attach(&p2e, new FixedConstraint(), CFrame(1.0, 0.0, 0.0), CFrame(0, 0, 0));

	Motion COMMotion(Vec3(1.0, 0.7, 1.3), Vec3(0, 0, 0));

	p1.getMainPhysical()->motionOfCenterOfMass = COMMotion;
	p1e.getMainPhysical()->motionOfCenterOfMass = COMMotion;

	ASSERT(p1.getMotion() == p1e.getMotion());
	ASSERT(p2.getMotion() == p2e.getMotion());
}

TEST_CASE(testMotionOfPhysicalJointsRotation) {
	Part p1(sphereShape(1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part p2(sphereShape(1.0), GlobalCFrame(1.0, 0.0, 0.0), {3.0, 1.0, 1.0});
	p1.attach(&p2, CFrame(1.0, 0.0, 0.0));

	Part p1e(sphereShape(1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part p2e(sphereShape(1.0), GlobalCFrame(1.0, 0.0, 0.0), {3.0, 1.0, 1.0});

	p1e.attach(&p2e, new FixedConstraint(), CFrame(1.0, 0.0, 0.0), CFrame(0.0, 0.0, 0.0));

	Motion COMMotion(Vec3(0, 0, 0), Vec3(-0.3, 1.7, -1.1));

	p1.getMainPhysical()->motionOfCenterOfMass = COMMotion;
	p1e.getMainPhysical()->motionOfCenterOfMass = COMMotion;

	logStream << p1.getPhysical()->getMotion() << "\n";
	logStream << p1.getMotion() << "\n";
	logStream << p2.getMotion() << "\n";
	logStream << p1e.getPhysical()->getMotion() << "\n";
	logStream << p1e.getMotion() << "\n";
	logStream << p2e.getMotion() << "\n";

	ASSERT(p1.getMotion() == p1e.getMotion());
	ASSERT(p2.getMotion() == p2e.getMotion());
}

TEST_CASE(testMotionOfPhysicalJoints) {
	Part p1(sphereShape(1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part p2(sphereShape(1.0), GlobalCFrame(1.0, 0.0, 0.0), {3.0, 1.0, 1.0});
	p1.attach(&p2, CFrame(1.0, 0.0, 0.0));

	Part p1e(sphereShape(1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part p2e(sphereShape(1.0), GlobalCFrame(1.0, 0.0, 0.0), {3.0, 1.0, 1.0});

	p1e.attach(&p2e, new FixedConstraint(), CFrame(1.0, 0.0, 0.0), CFrame(0, 0, 0));

	Motion COMMotion(Vec3(1.0, 0.7, 1.3), Vec3(-0.3, 1.7, -1.1));

	p1.getMainPhysical()->motionOfCenterOfMass = COMMotion;
	p1e.getMainPhysical()->motionOfCenterOfMass = COMMotion;

	logStream << p1.getPhysical()->getMotion() << "\n";
	logStream << p1.getMotion() << "\n";
	logStream << p2.getMotion() << "\n";
	logStream << p1e.getPhysical()->getMotion() << "\n";
	logStream << p1e.getMotion() << "\n";
	logStream << p2e.getMotion() << "\n";

	ASSERT(p1.getMotion() == p1e.getMotion());
	ASSERT(p2.getMotion() == p2e.getMotion());
}

TEST_CASE(testFixedConstraintProperties) {
	Part p1(sphereShape(1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part p2(sphereShape(1.0), GlobalCFrame(), {3.0, 1.0, 1.0});
	p1.attach(&p2, CFrame(1.0, 0.0, 0.0));

	Part p1e(sphereShape(1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part p2e(sphereShape(1.0), GlobalCFrame(), {3.0, 1.0, 1.0});

	p1e.attach(&p2e, new FixedConstraint(), CFrame(0.3, -0.8, 0.0), CFrame(-0.7, -0.8, 0));

	MotorizedPhysical* phys1 = p1.getMainPhysical();
	MotorizedPhysical* phys1e = p1e.getMainPhysical();

	ASSERT(p1.getCFrame() == GlobalCFrame(0.0, 0.0, 0.0));
	ASSERT(p2.getCFrame() == GlobalCFrame(1.0, 0.0, 0.0));
	ASSERT(p1e.getCFrame() == GlobalCFrame(0.0, 0.0, 0.0));
	ASSERT(p2e.getCFrame() == GlobalCFrame(1.0, 0.0, 0.0));
	ASSERT(phys1->totalMass == phys1e->totalMass);
	ASSERT(phys1->getCenterOfMass() == phys1e->getCenterOfMass());
	ASSERT(phys1->forceResponse == phys1e->forceResponse);
	ASSERT(phys1->momentResponse == phys1e->momentResponse);

}

TEST_CASE(testApplyForceToFixedConstraint) {
	CFrame attach(Vec3(1.3, 0.7, 0.9), Rotation::fromEulerAngles(0.3, -0.7, 0.9));

	Part p1(boxShape(1.0, 2.0, 3.0), GlobalCFrame(), {1.0, 1.0, 1.0});
	Part p2(boxShape(1.5, 2.3, 1.2), GlobalCFrame(), {1.0, 1.0, 1.0});

	p1.attach(&p2, attach);


	Part p1e(boxShape(1.0, 2.0, 3.0), GlobalCFrame(), {1.0, 1.0, 1.0});
	Part p2e(boxShape(1.5, 2.3, 1.2), GlobalCFrame(), {1.0, 1.0, 1.0});

	p1e.attach(&p2e, new FixedConstraint(), attach, CFrame());

	MotorizedPhysical* phys1 = p1.getMainPhysical();
	MotorizedPhysical* phys1e = p1e.getMainPhysical();

	phys1->applyForceAtCenterOfMass(Vec3(2.7, 3.9, -2.3));
	phys1e->applyForceAtCenterOfMass(Vec3(2.7, 3.9, -2.3));

	phys1->update(DELTA_T);
	phys1e->update(DELTA_T);

	ASSERT(phys1->getMotion() == phys1e->getMotion());

	ASSERT(p1.getCFrame() == p1e.getCFrame());
	ASSERT(p2.getCFrame() == p2e.getCFrame());

	ASSERT(phys1->forceResponse == phys1e->forceResponse);
	ASSERT(phys1->momentResponse == phys1e->momentResponse);

	phys1->applyImpulseAtCenterOfMass(Vec3(2.7, 3.9, -2.3));
	phys1e->applyImpulseAtCenterOfMass(Vec3(2.7, 3.9, -2.3));

	phys1->update(DELTA_T);
	phys1e->update(DELTA_T);

	ASSERT(phys1->getMotion() == phys1e->getMotion());

	ASSERT(p1.getCFrame() == p1e.getCFrame());
	ASSERT(p2.getCFrame() == p2e.getCFrame());

	ASSERT(phys1->forceResponse == phys1e->forceResponse);
	ASSERT(phys1->momentResponse == phys1e->momentResponse);
}

TEST_CASE(testPlainAttachAndFixedConstraintIndistinguishable) {
	Part firstPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part secondPart(boxShape(0.5, 0.5, 0.5), GlobalCFrame(1.0, 0.0, 0.0), {1.0, 1.0, 1.0});

	firstPart.attach(&secondPart, CFrame(1.0, 0.0, 0.0));

	Part firstPart2(boxShape(1.0, 1.0, 1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part secondPart2(boxShape(0.5, 0.5, 0.5), GlobalCFrame(1.0, 0.0, 0.0), {1.0, 1.0, 1.0});

	firstPart2.attach(&secondPart2, new FixedConstraint(), CFrame(0.5, 0.0, 0.0), CFrame(-0.5, 0.0, 0.0));

	Vec3 impulse(1.3, 4.7, 0.2);
	Vec3 impulsePos(2.6, 1.7, 2.8);

	MotorizedPhysical* main1 = firstPart.getMainPhysical();
	MotorizedPhysical* main2 = firstPart2.getMainPhysical();

	main1->applyImpulse(impulsePos, impulse);
	main2->applyImpulse(impulsePos, impulse);

	main1->update(0.5);
	main2->update(0.5);

	ASSERT(main1->getMotionOfCenterOfMass() == main2->getMotionOfCenterOfMass());
	ASSERT(main1->getCFrame() == main2->getCFrame());
}

TEST_CASE(testInternalMotionOfCenterOfMass) {
	Part firstPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	Part secondPart(boxShape(0.5, 0.5, 0.5), GlobalCFrame(1.0, 0.0, 0.0), {1.0, 1.0, 1.0});

	SinusoidalPistonConstraint* piston = new SinusoidalPistonConstraint(0.3, 1.0, 1.0);

	piston->currentStepInPeriod = 0.7;

	firstPart.attach(&secondPart, piston, CFrame(0.0, 0.0, 1.0), CFrame(0.0, 0.0, -1.0));

	MotorizedPhysical* phys = firstPart.getMainPhysical();

	Vec3 original = phys->totalCenterOfMass;

	ALLOCA_InternalMotionTree(cache, phys, size);

	TranslationalMotion motionOfCom = std::get<2>(cache.getInternalMotionOfCenterOfMass());

	phys->update(DELTA_T);
	Vec3 v2 = phys->totalCenterOfMass;

	phys->update(DELTA_T);
	Vec3 v3 = phys->totalCenterOfMass;

	TranslationalMotion estimatedMotion = estimateMotion(original, v2, v3, DELTA_T);

	ASSERT(motionOfCom == estimatedMotion);
}

static bool haveSameMotorPhys(const Part& first, const Part& second) {
	return first.getPhysical() != nullptr && second.getPhysical() != nullptr && first.getMainPhysical() == second.getMainPhysical();
}

TEST_CASE(attachPhysicalsNoLayers) {
	for(int iter = 0; iter < 100; iter++) {
		std::vector<Part> firstPhysParts = generateMotorizedPhysicalParts();
		std::vector<Part> secondPhysParts = generateMotorizedPhysicalParts();

		ASSERT_FALSE(haveSameMotorPhys(firstPhysParts[0], secondPhysParts[0]));

		generateAttachment(oneOf(firstPhysParts), oneOf(secondPhysParts));

		ASSERT_TRUE(haveSameMotorPhys(firstPhysParts[0], secondPhysParts[0]));
	}
}

bool areInSameGroup(const Part& a, const Part& b) {
	return a.layer->tree.groupContains(&a, &b);
}

static bool pairwiseCorrectlyGrouped(const std::vector<Part>& la, const std::vector<Part>& lb, bool expectedAreInSameGroup) {
	for(const Part& a : la) {
		for(const Part& b : lb) {
			if(a.layer == b.layer) {
				if(areInSameGroup(a, b) != expectedAreInSameGroup) {
					return false;
				}
			}
		}
	}
	return true;
}

TEST_CASE(attachPhysicalsWithLayers) {
	for(int iter = 0; iter < 100; iter++) {
		WorldPrototype testWorld(0.005);
		testWorld.createLayer(true, true);
		testWorld.createLayer(false, false);
		std::vector<Part> firstPhysParts = generateMotorizedPhysicalParts();
		std::vector<Part> secondPhysParts = generateMotorizedPhysicalParts();
		generateLayerAssignment(firstPhysParts, testWorld);
		generateLayerAssignment(secondPhysParts, testWorld);

		ASSERT_FALSE(haveSameMotorPhys(firstPhysParts[0], secondPhysParts[0]));

		ASSERT_TRUE(pairwiseCorrectlyGrouped(firstPhysParts, firstPhysParts, true));
		ASSERT_TRUE(pairwiseCorrectlyGrouped(secondPhysParts, secondPhysParts, true));
		ASSERT_TRUE(pairwiseCorrectlyGrouped(firstPhysParts, secondPhysParts, false));

		Part& attachedPart1 = oneOf(firstPhysParts);
		Part& attachedPart2 = oneOf(secondPhysParts);

		generateAttachment(attachedPart1, attachedPart2);

		ASSERT_TRUE(haveSameMotorPhys(firstPhysParts[0], secondPhysParts[0]));

		ASSERT_TRUE(pairwiseCorrectlyGrouped(firstPhysParts, firstPhysParts, true));
		ASSERT_TRUE(pairwiseCorrectlyGrouped(secondPhysParts, secondPhysParts, true));
		ASSERT_TRUE(pairwiseCorrectlyGrouped(firstPhysParts, secondPhysParts, true));
	}
}

