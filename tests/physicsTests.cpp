#include "testsMain.h"

#include "compare.h"
#include "../physics/misc/toString.h"
#include "simulation.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "../physics/world.h"
#include "../physics/inertia.h"
#include "../physics/misc/shapeLibrary.h"
#include "../physics/math/linalg/trigonometry.h"
#include "../physics/math/linalg/eigen.h"
#include "../physics/geometry/shape.h"
#include "../physics/geometry/shapeCreation.h"
#include "../physics/misc/gravityForce.h"
#include "../physics/constraints/motorConstraint.h"
#include "../physics/constraints/sinusoidalPistonConstraint.h"
#include "../physics/constraints/fixedConstraint.h"
#include "../util/log.h"


#define REMAINS_CONSTANT(v) REMAINS_CONSTANT_TOLERANT(v, 0.0005)
#define ASSERT(v) ASSERT_TOLERANT(v, 0.0005)

static const double DELTA_T = 0.01;
static const int TICKS = 300;

static const PartProperties basicProperties{0.7, 0.2, 0.6};

TEST_CASE_SLOW(positionInvariance) {
	Rotation rotation = Rotation::fromEulerAngles(0.0, 0.0, 0.0);

	Position origins[]{Position(0,0,0), Position(0,0,1), Position(5.3,0,-2.4),Position(0.3,0,0.7),Position(0.0000001,0,0.00000001)};

	CFrame houseRelative(Vec3(0.7, 0.6, 1.6), Rotation::fromEulerAngles(0.3, 0.7, 0.9));
	CFrame icosaRelative(Vec3(0.7, 3.0, 1.6), Rotation::fromEulerAngles(0.1, 0.1, 0.1));

	for(Position o:origins) {
		GlobalCFrame origin(o, rotation);

		WorldPrototype world(DELTA_T);
		world.addExternalForce(new DirectionalGravity(Vec3(0, -1, 0)));

		Part housePart(polyhedronShape(Library::house), origin.localToGlobal(houseRelative), {1.0, 1.0, 0.7});
		Part icosaPart(polyhedronShape(Library::icosahedron), origin.localToGlobal(icosaRelative), {10.0, 0.7, 0.7});
		Part flooring(boxShape(200.0, 0.3, 200.0), origin, {1.0, 1.0, 0.7});

		world.addPart(&housePart);
		world.addPart(&icosaPart);
		world.addTerrainPart(&flooring);

		for(int i = 0; i < TICKS; i++)
			world.tick();

		REMAINS_CONSTANT_TOLERANT(origin.globalToLocal(housePart.getCFrame()), 0.0002);
		REMAINS_CONSTANT_TOLERANT(origin.globalToLocal(icosaPart.getCFrame()), 0.0002);
	}
}

TEST_CASE_SLOW(rotationInvariance) {
	CFrame houseRelative(Vec3(0.7, 0.6, 1.6), Rotation::fromEulerAngles(0.3, 0.7, 0.9));
	CFrame icosaRelative(Vec3(0.7, 3.0, 1.6), Rotation::fromEulerAngles(0.1, 0.1, 0.1));

	for(double rotation = -1.5; rotation < 1.500001; rotation += 0.3) {
		logStream << rotation << '\n';

		GlobalCFrame origin(Position(0.0,0.0,0.0), Rotation::rotY(rotation));

		WorldPrototype world(DELTA_T);
		world.addExternalForce(new DirectionalGravity(Vec3(0, -1, 0)));

		Part housePart(polyhedronShape(Library::house), origin.localToGlobal(houseRelative), {1.0, 1.0, 0.7});
		Part icosaPart(polyhedronShape(Library::icosahedron), origin.localToGlobal(icosaRelative), {10.0, 0.7, 0.7});
		Part flooring(boxShape(200.0, 0.3, 200.0), origin, {1.0, 1.0, 0.7});

		world.addPart(&housePart);
		world.addPart(&icosaPart);
		world.addTerrainPart(&flooring);

		for(int i = 0; i < TICKS; i++)
			world.tick();

		REMAINS_CONSTANT_TOLERANT(origin.globalToLocal(housePart.getCFrame()), 0.02);
		REMAINS_CONSTANT_TOLERANT(origin.globalToLocal(icosaPart.getCFrame()), 0.02);
	}
}

TEST_CASE(applyForceToRotate) {
	Part part(boxShape(1.0, 1.0, 1.0), GlobalCFrame(0,0,0, Rotation::fromEulerAngles(0.3, 0.7, 0.9)), {1.0, 1.0, 0.7});
	part.ensureHasParent();

	Vec3 relAttach = Vec3(1.0, 0.0, 0.0);
	Vec3 force = Vec3(0.0, 1.0, 0.0);

	part.parent->mainPhysical->applyForce(relAttach, force);
	ASSERT(part.parent->mainPhysical->totalForce == force);
	ASSERT(part.parent->mainPhysical->totalMoment == Vec3(0.0, 0.0, 1.0));
}

TEST_CASE(momentToAngularVelocity) {
	Part part(boxShape(1.0, 1.0, 1.0), GlobalCFrame(Rotation::rotY(M_PI / 2)), {1.0, 1.0, 0.7});
	part.ensureHasParent();
	MotorizedPhysical& p = *part.parent->mainPhysical;

	Vec3 moment(1.0, 0.0, 0.0);

	for(int i = 0; i < 50; i++) {
		p.applyMoment(moment);
		p.update(0.05);
	}

	ASSERT(p.getMotion().getAngularVelocity() == moment * (50.0 * 0.05) * p.momentResponse(0, 0));
}

TEST_CASE(rotationImpulse) {
	Part part(boxShape(0.2, 20.0, 0.2), GlobalCFrame(0,0,0), {1.0, 1.0, 0.7});
	part.ensureHasParent();
	MotorizedPhysical& veryLongBoxPhysical = *part.parent->mainPhysical;

	Vec3 xMoment = Vec3(1.0, 0.0, 0.0);
	Vec3 yMoment = Vec3(0.0, 1.0, 0.0);
	Vec3 zMoment = Vec3(0.0, 0.0, 1.0);

	{
		veryLongBoxPhysical.totalForce = Vec3();
		veryLongBoxPhysical.totalMoment = Vec3();
	}
}

/*TEST_CASE(testPointAcceleration) {
	Part testPart(boxShape(1.0, 2.0, 3.0), GlobalCFrame(0,0,0), {1.0, 1.0, 0.7});
	testPart.ensureHasParent();
	MotorizedPhysical& testPhys = *testPart.parent->mainPhysical;
	Vec3 localPoint(3, 5, 7);
	Vec3 force(-4, -3, 0.5);
	double deltaT = 0.00001;

	testPhys.applyForce(localPoint, force);

	Vec3 acceleration = testPhys.getMotion().getAcceleration();
	Vec3 angularAcceleration = testPhys.getMotion().rotation.angularAcceleration;
	Vec3 pointAcceleration = testPhys.getMotion().getAccelerationOfPoint(localPoint);

	testPhys.update(deltaT);

	Vec3 actualAcceleration = testPhys.getMotion().getVelocity() / deltaT;
	Vec3 actualAngularAcceleration = testPhys.getMotion().rotation.angularVelocity / deltaT;
	Vec3 actualPointAcceleration = testPhys.getMotion().getVelocityOfPoint(testPhys.getCFrame().localToRelative(localPoint)) / deltaT;

	ASSERT(acceleration == actualAcceleration);
	ASSERT(angularAcceleration == actualAngularAcceleration);
	ASSERT(pointAcceleration == actualPointAcceleration);
}*/

/*TEST_CASE(testGetPointAccelerationMatrix) {
	Part testPart(boxShape(1.0, 2.0, 3.0), GlobalCFrame(0,0,0), {1.0, 1.0, 0.7});
	testPart.ensureHasParent();
	MotorizedPhysical& testPhys = *testPart.parent->mainPhysical;
	Vec3 localPoint(3, 5, 7);
	Vec3 force(-4, -3, 0.5);

	testPhys.applyForce(localPoint, force);

	SymmetricMat3 accelMatrix = testPhys.getResponseMatrix(localPoint);

	logStream << accelMatrix;

	Vec3 actualAcceleration = testPhys.getMotion().getAccelerationOfPoint(localPoint);

	ASSERT(actualAcceleration == accelMatrix * force);
}*/
TEST_CASE(impulseTest) {
	Part part(boxShape(1.0, 2.0, 2.5), GlobalCFrame(0,0,0), {1.0, 1.0, 0.7});
	part.ensureHasParent();
	MotorizedPhysical& p = *part.parent->mainPhysical;

	p.applyImpulseAtCenterOfMass(Vec3(15, 0, 0));
	ASSERT(p.getMotion().getVelocity() == Vec3(3,0,0));
	ASSERT(p.getMotion().getAngularVelocity() == Vec3(0, 0, 0));

	Vec3 angularImpulse = Vec3(0, 2, 0) % Vec3(-15, 0, 0);

	p.applyImpulse(Vec3(0, 2, 0), Vec3(-15, 0, 0));
	ASSERT(p.getMotion().getVelocity() == Vec3(0, 0, 0));
	ASSERT(p.getMotion().getAngularVelocity() == ~part.getInertia() * angularImpulse);
}

TEST_CASE(testPointAccelMatrixImpulse) {
	Part part(boxShape(1.0, 2.0, 3.0), GlobalCFrame(7.6, 3.4, 3.9, Rotation::fromEulerAngles(1.1, 0.7, 0.9)), {1.0, 1.0, 0.7});
	part.ensureHasParent();
	MotorizedPhysical& p = *part.parent->mainPhysical;

	Vec3 localPoint(0.8, 0.6, 0.9);
	Vec3 localImpulse(0.3, -0.7, 0.6);

	Vec3 estimatedAccel = p.getResponseMatrix(localPoint) * localImpulse;

	p.applyImpulse(part.getCFrame().localToRelative(localPoint), part.getCFrame().localToRelative(localImpulse));

	Vec3 realAccel = part.getCFrame().relativeToLocal(p.getMotion().getVelocityOfPoint(part.getCFrame().localToRelative(localPoint)));

	ASSERT(estimatedAccel == realAccel);
}

TEST_CASE(inelasticColission) {
	Part part(boxShape(1.0, 2.0, 3.0), GlobalCFrame(7.6, 3.4, 3.9, Rotation::fromEulerAngles(1.1, 0.7, 0.9)), {1.0, 1.0, 0.7});
	part.ensureHasParent();
	MotorizedPhysical& p = *part.parent->mainPhysical;

	Vec3 localPoint(0.8, 0.6, 0.9);
	Vec3 relativePoint = p.getCFrame().localToRelative(localPoint);

	p.getMotion().getVelocity() = Vec3(0.3, -1.3, 1.2);
	p.getMotion().getAngularVelocity() = Vec3(0.7, 0.5, -0.9);

	Vec3 velOfPoint = p.getMotion().getVelocityOfPoint(relativePoint);

	ASSERT(velOfPoint.y < 0);

	logStream << "totalVelocity: " << str(velOfPoint);

	Vec3 direction(0.0, 170.0, 0.0);

	//double inertia = p.getInertiaOfPointInDirection(localPoint, p.getCFrame().relativeToLocal(direction));

	//Log::warn("inertia: %f", inertia);

	logStream << "velInDirection: " << velOfPoint * normalize(direction);

	//Vec3 relativeImpulse = -velOfPoint * direction.normalize() * direction.normalize() * inertia;

	Vec3 desiredAccel = -velOfPoint * direction * direction / lengthSquared(direction);
	Vec3 relativeImpulse = p.getCFrame().localToRelative(~p.getResponseMatrix(localPoint) * p.getCFrame().relativeToLocal(desiredAccel));
	Vec3 estimatedAccelLocal = p.getResponseMatrix(localPoint) * p.getCFrame().relativeToLocal(relativeImpulse);

	Vec3 estimatedAccelRelative = p.getCFrame().localToRelative(estimatedAccelLocal);
	

	p.applyImpulse(relativePoint, relativeImpulse);
	

	Vec3 velOfPointAfter = p.getMotion().getVelocityOfPoint(relativePoint);
	logStream << "New velocity: " << str(velOfPointAfter);
	logStream << "velInDirection After: " << velOfPointAfter * normalize(direction);
	logStream << "estimatedAccelRelative: " << str(estimatedAccelRelative);
	logStream << "Actual accel:           " << str(velOfPointAfter - velOfPoint);

	ASSERT(estimatedAccelRelative == velOfPointAfter - velOfPoint);
	ASSERT(velOfPointAfter.y == 0);
}

TEST_CASE(inelasticColission2) {
	Part part(boxShape(1.0, 2.0, 3.0), GlobalCFrame(/*Vec3(7.6, 3.4, 3.9), rotationMatrixfromEulerAngles(1.1, 0.7, 0.9)*/), {1.0, 1.0, 0.7});
	part.ensureHasParent();
	MotorizedPhysical& p = *part.parent->mainPhysical;

	Vec3 localPoint(0.8, 0.6, 0.9);
	Vec3 relativePoint = p.getCFrame().localToRelative(localPoint);
	Vec3 normal(0.0, 170.0, 0.0);

	p.getMotion().getVelocity() = Vec3(0.3, -1.3, 1.2);
	p.getMotion().getAngularVelocity() = Vec3(0.7, 0.5, -0.9);

	Vec3 velOfPoint = p.getMotion().getVelocityOfPoint(relativePoint);

	ASSERT(velOfPoint.y < 0);

	double inertia = p.getInertiaOfPointInDirectionRelative(localPoint, normal);

	double normalVelocity = velOfPoint * normalize(normal);

	double desiredAccel = -normalVelocity;

	Vec3 impulse = normalize(normal) * desiredAccel * inertia;



	p.applyImpulse(relativePoint, impulse);


	Vec3 velOfPointAfter = p.getMotion().getVelocityOfPoint(relativePoint);
	logStream << "New velocity: " + str(velOfPointAfter);
	logStream << "velInDirection After: ", velOfPointAfter * normalize(normal);
	//logStream << "estimatedAccelRelative: " + str(estimatedAccelRelative);
	logStream << "Actual accel:           " + str(velOfPointAfter - velOfPoint);

	//ASSERT(estimatedAccelRelative == velOfPointAfter - velOfPoint);
	ASSERT(velOfPointAfter.y == 0);
}

/*TEST_CASE(testPointAccelMatrixAndInertiaInDirection) {
	Part part(boxShape(1.0, 1.0, 1.0), GlobalCFrame(Position(7.6, 3.4, 3.9), Rotation::fromEulerAngles(1.1, 0.7, 0.9)), {1.0, 1.0, 0.7});
	MotorizedPhysical p(&part);

	Vec3 localPoint(0.8, 0.6, 0.9);
	Vec3 localImpulse(0.3, -0.7, 0.6);

	Vec3 estimatedAccel = p.getResponseMatrix(localPoint) * localImpulse;

	p.applyImpulse(part.getCFrame().localToRelative(localPoint), part.getCFrame().localToRelative(localImpulse));

	Vec3 realAccel = part.getCFrame().relativeToLocal(p.getVelocityOfPoint(part.getCFrame().localToRelative(localPoint)));

	ASSERT(estimatedAccel == realAccel);
}*/

TEST_CASE(testChangeInertialBasis) {
	Rotation rotation = Rotation::fromEulerAngles(0.6, 0.3, 0.7);
	Polyhedron rotatedTriangle = Library::trianglePyramid.rotated(static_cast<Rotationf>(rotation));
	SymmetricMat3 triangleInertia = Library::trianglePyramid.getInertia(CFrame());
	SymmetricMat3 rotatedTriangleInertia = rotatedTriangle.getInertia(CFrame());

	ASSERT(getEigenDecomposition(triangleInertia).eigenValues == getEigenDecomposition(rotatedTriangleInertia).eigenValues);
	ASSERT(getRotatedInertia(triangleInertia, rotation) == rotatedTriangleInertia);
}

TEST_CASE(testMultiPartPhysicalSimple) {
	Shape box(boxShape(1.0, 0.5, 0.5));
	Shape box2(boxShape(1.0, 0.5, 0.5));
	Shape doubleBox(boxShape(2.0, 0.5, 0.5));
	Part p1(box, GlobalCFrame(), {10.0, 0.5, 0.5});
	Part p2(box2, GlobalCFrame(), {10.0, 0.5, 0.5});
	Part doubleP(doubleBox, GlobalCFrame(), {10.0, 0.5, 0.5});

	p1.ensureHasParent();
	MotorizedPhysical& phys = *p1.parent->mainPhysical;
	phys.attachPart(&p2, CFrame(Vec3(1.0, 0.0, 0.0)));

	doubleP.ensureHasParent();
	MotorizedPhysical& phys2 = *doubleP.parent->mainPhysical;

	ASSERT(phys.totalMass == p1.getMass() + p2.getMass());
	ASSERT(phys.totalCenterOfMass == Vec3(0.5, 0, 0));
	ASSERT(phys.forceResponse == phys2.forceResponse);
	ASSERT(phys.momentResponse == phys2.momentResponse);
}

TEST_CASE(testMultiPartPhysicalRotated) {
	Shape box(boxShape(1.0, 0.5, 0.5));
	Shape box2(boxShape(0.5, 0.5, 1.0));
	Shape doubleBox(boxShape(2.0, 0.5, 0.5));
	Part* p1 = new Part(box, GlobalCFrame(), {10.0, 0.0, 0.7});
	Part* p2 = new Part(box2, GlobalCFrame(), {10.0, 0.0, 0.7});
	Part* doubleP = new Part(doubleBox, GlobalCFrame(), {10.0, 0, 0.7});

	MotorizedPhysical phys(p1);
	phys.attachPart(p2, CFrame(Vec3(1.0, 0.0, 0.0), Rotation::Predefined::Y_90));

	MotorizedPhysical phys2(doubleP);

	ASSERT(phys.totalMass == p1->getMass() + p2->getMass());
	ASSERT(phys.totalCenterOfMass == Vec3(0.5, 0, 0));
	ASSERT(phys.forceResponse == phys2.forceResponse);
	ASSERT(phys.momentResponse == phys2.momentResponse);
}

TEST_CASE(testShapeNativeScaling) {
	Polyhedron testPoly = Library::createPointyPrism(4, 1.0f, 1.0f, 0.5f, 0.5f);

	Shape shape1(polyhedronShape(testPoly));
	Shape shape2 = shape1.scaled(2.0, 4.0, 3.7);

	Polyhedron scaledTestPoly = testPoly.scaled(2.0f, 4.0f, 3.7f);

	ASSERT(shape2.getInertia() == scaledTestPoly.getInertiaAroundCenterOfMass());
}

TEST_CASE(testPhysicalInertiaDerivatives) {
	Polyhedron testPoly = Library::createPointyPrism(4, 1.0f, 1.0f, 0.5f, 0.5f);

	Part mainPart(polyhedronShape(testPoly), GlobalCFrame(), basicProperties);
	Part part1_mainPart(polyhedronShape(Library::house), mainPart, 
						new SinusoidalPistonConstraint(0.0, 2.0, 1.3), 
						CFrame(0.3, 0.7, -0.5, Rotation::fromEulerAngles(0.7, 0.3, 0.7)), 
						CFrame(0.1, 0.2, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)), basicProperties);
	Part part2_mainPart(boxShape(1.0, 0.3, 2.0), mainPart, 
						new MotorConstraintTemplate<ConstantMotorTurner>(1.7),
						CFrame(-0.3, 0.7, 0.5, Rotation::fromEulerAngles(0.7, 0.3, 0.7)), 
						CFrame(0.1, -0.2, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)), basicProperties);
	Part part1_part1_mainPart(cylinderShape(1.0, 0.3), part1_mainPart, 
						new MotorConstraintTemplate<ConstantMotorTurner>(1.3),
						CFrame(-0.3, 0.7, 0.5, Rotation::fromEulerAngles(0.7, 0.3, 0.7)),
						CFrame(0.1, -0.2, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)) , basicProperties);
	Part part1_part1_part1_mainPart(polyhedronShape(Library::trianglePyramid), part1_part1_mainPart, 
						new SinusoidalPistonConstraint(0.0, 2.0, 1.3),
						CFrame(0.3, 0.7, -0.5, Rotation::fromEulerAngles(0.7, 0.3, 0.7)),
						CFrame(0.1, 0.2, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)), basicProperties);

	MotorizedPhysical* motorPhys = mainPart.parent->mainPhysical;

	std::size_t size = motorPhys->getNumberOfPhysicalsInThisAndChildren() - 1;
	UnmanagedArray<MonotonicTreeNode<RelativeMotion>> arr(new MonotonicTreeNode<RelativeMotion>[size], size);
	FullTaylor<SymmetricMat3> inertiaTaylor = motorPhys->getCOMMotionTree(std::move(arr)).getInertiaDerivatives();

	double deltaT = 0.00001;

	std::array<SymmetricMat3, 3> inertias;
	inertias[0] = motorPhys->getCOMMotionTree(std::move(arr)).getInertia();
	motorPhys->update(deltaT);
	inertias[1] = motorPhys->getCOMMotionTree(std::move(arr)).getInertia();
	motorPhys->update(deltaT);
	inertias[2] = motorPhys->getCOMMotionTree(std::move(arr)).getInertia();

	delete[] arr.getPtrToFree();

	FullTaylor<SymmetricMat3> estimatedInertiaTaylor = estimateDerivatives(inertias, deltaT);

	ASSERT_TOLERANT(inertiaTaylor == estimatedInertiaTaylor, 0.01);
}

TEST_CASE(testCenterOfMassKept) {
	Polyhedron testPoly = Library::createPointyPrism(4, 1.0f, 1.0f, 0.5f, 0.5f);

	Part mainPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(), basicProperties);
	Part part1_mainPart(boxShape(1.0, 1.0, 1.0), mainPart,
						new SinusoidalPistonConstraint(0.0, 2.0, 1.0),
						CFrame(0.0, 0.0, 0.0, Rotation::Predefined::IDENTITY),
						CFrame(0.0, 0.0, 0.0, Rotation::Predefined::IDENTITY), basicProperties);

	ALLOCA_COMMotionTree(t, mainPart.parent->mainPhysical, size);

	logStream << t.getRelativePosOfMain();

	ASSERT(t.getRelativePosOfMain() == -t.relativeMotionTree[0].value.locationOfRelativeMotion.getPosition());
	ASSERT(t.getLocalMotionOfMain() == -t.relativeMotionTree[0].value.relativeMotion.translation);
}

TEST_CASE(testBasicAngularMomentum) {
	double motorSpeed = 1.0;
	MotorConstraintTemplate<ConstantMotorTurner>* constraint = new MotorConstraintTemplate<ConstantMotorTurner>(motorSpeed);

	Part mainPart(cylinderShape(1.0, 1.0), GlobalCFrame(), basicProperties);
	Part attachedPart(cylinderShape(1.0, 1.0), mainPart,
					  constraint,
						CFrame(0.0, 0.0, 0.0, Rotation::Predefined::IDENTITY),
						CFrame(0.0, 0.0, 0.0, Rotation::Predefined::IDENTITY), basicProperties);

	ALLOCA_COMMotionTree(t, mainPart.parent->mainPhysical, size);

	SymmetricMat3 inertia = attachedPart.getInertia();
	Vec3 angularVel = constraint->getRelativeMotion().relativeMotion.getAngularVelocity();
	Vec3 angularMomentum = inertia * angularVel;

	ASSERT(t.getInternalAngularMomentum() == angularMomentum);
}

TEST_CASE(testBasicAngularMomentumTurned) {
	double motorSpeed = 1.0;
	MotorConstraintTemplate<ConstantMotorTurner>* constraint = new MotorConstraintTemplate<ConstantMotorTurner>(motorSpeed);

	Part mainPart(cylinderShape(1.0, 1.0), GlobalCFrame(), basicProperties);
	Part attachedPart(cylinderShape(1.0, 1.0), mainPart,
					  constraint,
					  CFrame(0.0, 0.0, 0.0, Rotation::Predefined::Y_90),
					  CFrame(0.0, 0.0, 0.0, Rotation::Predefined::IDENTITY), basicProperties);

	
	ALLOCA_COMMotionTree(t, mainPart.parent->mainPhysical, size);

	SymmetricMat3 inertia = attachedPart.getInertia();
	Vec3 angularVel = constraint->getRelativeMotion().relativeMotion.getAngularVelocity();
	Vec3 angularMomentum = Rotation::Predefined::Y_90.localToGlobal(inertia * angularVel);

	ASSERT(t.getInternalAngularMomentum() == angularMomentum);
}

TEST_CASE(testFixedConstraintAngularMomentum) {
	double motorSpeed = 1.0;

	
	MotorConstraintTemplate<ConstantMotorTurner>* constraint1 = new MotorConstraintTemplate<ConstantMotorTurner>(motorSpeed);

	double offset = 5.0;

	Part mainPart1(cylinderShape(1.0, 1.0), GlobalCFrame(), basicProperties);
	Part attachedPart1(cylinderShape(1.0, 1.0), mainPart1,
						constraint1,
						CFrame(0.0, 0.0, 0.0, Rotation::Predefined::IDENTITY),
						CFrame(0.0, 0.0, 0.0, Rotation::Predefined::IDENTITY), basicProperties);
	Part attachedPart1A(boxShape(1.0, 1.0, 1.0), attachedPart1,
						new FixedConstraint(),
						CFrame(0.0, 0.0, 0.0, Rotation::Predefined::IDENTITY),
						CFrame(offset, 0.0, 0.0, Rotation::Predefined::IDENTITY), basicProperties);
	Part attachedPart1B(boxShape(1.0, 1.0, 1.0), attachedPart1,
						new FixedConstraint(),
						CFrame(0.0, 0.0, 0.0, Rotation::Predefined::IDENTITY),
						CFrame(-offset, 0.0, 0.0, Rotation::Predefined::IDENTITY), basicProperties);

	ALLOCA_COMMotionTree(t1, mainPart1.parent->mainPhysical, size1);

	MotorConstraintTemplate<ConstantMotorTurner>* constraint2 = new MotorConstraintTemplate<ConstantMotorTurner>(motorSpeed);

	Part mainPart2(cylinderShape(1.0, 1.0), GlobalCFrame(), basicProperties);
	Part attachedPart2(cylinderShape(1.0, 1.0), mainPart2,
						constraint2,
						CFrame(0.0, 0.0, 0.0, Rotation::Predefined::IDENTITY),
						CFrame(0.0, 0.0, 0.0, Rotation::Predefined::IDENTITY), basicProperties);
	Part attachedPart2A(boxShape(1.0, 1.0, 1.0), attachedPart2,
						CFrame(offset, 0.0, 0.0, Rotation::Predefined::IDENTITY), basicProperties);
	Part attachedPart2B(boxShape(1.0, 1.0, 1.0), attachedPart2,
						CFrame(-offset, 0.0, 0.0, Rotation::Predefined::IDENTITY), basicProperties);

	ALLOCA_COMMotionTree(t2, mainPart2.parent->mainPhysical, size2);
	
	ASSERT(t1.totalMass == t2.totalMass);
	ASSERT(t1.centerOfMass == t2.centerOfMass);
	ASSERT(t1.localMotionOfCenterOfMass == t2.localMotionOfCenterOfMass);
	ASSERT(t1.getInertia() == t2.getInertia());
	ASSERT(t1.getInertiaDerivatives() == t2.getInertiaDerivatives());
	ASSERT(t1.getInternalAngularMomentum() == t2.getInternalAngularMomentum());
}

static Position getTotalCenterOfMassOfPhysical(const MotorizedPhysical* motorPhys) {
	double totalMass = 0.0;
	Vec3 totalCenterOfMass(0.0, 0.0, 0.0);

	motorPhys->forEachPart([&totalMass, &totalCenterOfMass](const Part& p) {
		const GlobalCFrame& pcf = p.getCFrame();
		const Vec3 pcom = p.getCenterOfMass() - Position(0,0,0);
		totalCenterOfMass += pcom * p.getMass();
		totalMass += p.getMass();
	});

	return Position(0, 0, 0) + totalCenterOfMass / totalMass;
}

static Vec3 getTotalMotionOfCenterOfMassOfPhysical(const MotorizedPhysical* motorPhys) {
	double totalMass = 0.0;
	Vec3 totalVelocityOfCenterOfMass(0.0, 0.0, 0.0);

	motorPhys->forEachPart([&totalMass, &totalVelocityOfCenterOfMass](const Part& p) {
		const GlobalCFrame& pcf = p.getCFrame();
		Vec3 relativePartCOM = pcf.localToRelative(p.getLocalCenterOfMass());
		Motion m = p.getMotion().getMotionOfPoint(relativePartCOM);
		Vec3 relVel = m.getVelocity();
		totalVelocityOfCenterOfMass += relVel * p.getMass();
		totalMass += p.getMass();
	});

	return totalVelocityOfCenterOfMass / totalMass;
}

static SymmetricMat3 getTotalInertiaOfPhysical(const MotorizedPhysical* motorPhys) {
	SymmetricMat3 totalInertia{
		0.0, 
		0.0, 0.0, 
		0.0, 0.0, 0.0
	};
	Position com = motorPhys->getCenterOfMass();

	motorPhys->forEachPart([&com, &totalInertia](const Part& p) {
		const GlobalCFrame& pcf = p.getCFrame();
		SymmetricMat3 globalInertia = pcf.getRotation().localToGlobal(p.getInertia());
		Vec3 relativePartCOM = pcf.localToRelative(p.getLocalCenterOfMass());
		Vec3 offset = p.getCenterOfMass() - com;
		SymmetricMat3 inertiaOfThis = getTranslatedInertiaAroundCenterOfMass(globalInertia, p.getMass(), offset);
		totalInertia += inertiaOfThis;
	});

	return totalInertia;
}

static Vec3 getTotalAngularMomentumOfPhysical(const MotorizedPhysical* motorPhys) {
	Vec3 totalAngularMomentum(0.0, 0.0, 0.0);
	Position com = motorPhys->getCenterOfMass();
	Vec3 comVel = motorPhys->getMotionOfCenterOfMass().getVelocity();

	motorPhys->forEachPart([&com, &comVel, &totalAngularMomentum](const Part& p) {
		const GlobalCFrame& pcf = p.getCFrame();
		SymmetricMat3 globalInertia = pcf.getRotation().localToGlobal(p.getInertia());
		Vec3 relativePartCOM = pcf.localToRelative(p.getLocalCenterOfMass());
		Vec3 offset = p.getCenterOfMass() - com;
		Motion m = p.getMotion().getMotionOfPoint(relativePartCOM);
		Vec3 relVel = m.getVelocity();
		Vec3 angMomOfThis = getAngularMomentumFromOffset(offset, relVel, m.getAngularVelocity(), globalInertia, p.getMass());
		totalAngularMomentum += angMomOfThis;
	});

	return totalAngularMomentum;
}

std::vector<Part> producePhysical() {
	std::vector<Part> result;
	result.reserve(10);
	Part& mainPart = result.emplace_back(polyhedronShape(Library::house.rotated(Rotationf::fromEulerAngles(1.0f, -0.3f, 0.5f))), GlobalCFrame(), PartProperties{0.7, 0.2, 0.6});

	mainPart.ensureHasParent();

	Part& part1_mainPart = result.emplace_back(polyhedronShape(Library::house), mainPart,
											CFrame(0.3, 0.7, -0.5, Rotation::fromEulerAngles(0.7, 0.3, 0.7)), basicProperties);
	Part& part2_mainPart = result.emplace_back(boxShape(1.0, 0.3, 2.0), mainPart,
											CFrame(0.1, -0.2, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)), basicProperties);
	Part& part1_part1_mainPart = result.emplace_back(cylinderShape(1.0, 0.3), part1_mainPart,
											CFrame(0.1, -0.2, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)), basicProperties);
	Part& part1_part1_part1_mainPart = result.emplace_back(polyhedronShape(Library::trianglePyramid), part1_part1_mainPart,
											CFrame(0.1, 0.2, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)), basicProperties);

	return result;
}

std::vector<Part> produceMotorizedPhysical() {
	std::vector<Part> result;
	result.reserve(10);
	Part& mainPart = result.emplace_back(polyhedronShape(Library::house.rotated(Rotationf::fromEulerAngles(1.0f, -0.3f, 0.5f))), GlobalCFrame(), PartProperties{0.7, 0.2, 0.6});

	mainPart.ensureHasParent();

	Part& part1_mainPart = result.emplace_back(polyhedronShape(Library::house), mainPart,
						new SinusoidalPistonConstraint(0.0, 2.0, 1.3),
						CFrame(0.3, 0.7, -0.5, Rotation::fromEulerAngles(0.7, 0.3, 0.7)),
						CFrame(0.1, 0.2, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)), basicProperties);
	Part& part2_mainPart = result.emplace_back(boxShape(1.0, 0.3, 2.0), mainPart,
						new MotorConstraintTemplate<ConstantMotorTurner>(1.7),
						CFrame(-0.3, 0.7, 0.5, Rotation::fromEulerAngles(0.7, 0.3, 0.7)),
						CFrame(0.1, -0.2, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)), basicProperties);
	Part& part1_part1_mainPart = result.emplace_back(cylinderShape(1.0, 0.3), part1_mainPart,
							  new MotorConstraintTemplate<ConstantMotorTurner>(1.3),
							  CFrame(-0.3, 0.7, 0.5, Rotation::fromEulerAngles(0.7, 0.3, 0.7)),
							  CFrame(0.1, -0.2, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)), basicProperties);
	Part& part1_part1_part1_mainPart = result.emplace_back(polyhedronShape(Library::trianglePyramid), part1_part1_mainPart,
									new SinusoidalPistonConstraint(0.0, 2.0, 1.3),
									CFrame(0.3, 0.7, -0.5, Rotation::fromEulerAngles(0.7, 0.3, 0.7)),
									CFrame(0.1, 0.2, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)), basicProperties);

	return result;
}

struct FullGlobalDiagnostic {
	Motion motionOfCOM;
	Vec3 getVelocity;
	double kineticEnergy;
	double inertiaInDirection;
	Vec3 getTotalAngularMomentum;
	Position positions[TICKS];
};

template<typename Tol>
bool tolerantEquals(const FullGlobalDiagnostic& first, const FullGlobalDiagnostic& second, Tol tolerance) {
	if(tolerantEquals(first.motionOfCOM, second.motionOfCOM, tolerance) &&
	   tolerantEquals(first.getVelocity, second.getVelocity, tolerance) &&
	   tolerantEquals(first.kineticEnergy, second.kineticEnergy, tolerance) &&
	   tolerantEquals(first.inertiaInDirection, second.inertiaInDirection, tolerance) &&
	   tolerantEquals(first.getTotalAngularMomentum, second.getTotalAngularMomentum, tolerance)) {

		for(std::size_t i = 0; i < TICKS; i++) {
			if(!tolerantEquals(first.positions[i], second.positions[i], tolerance)) {
				return false;
			}
		}
		return true;
	} else {
		return false;
	}
}

std::ostream& operator<<(std::ostream& ostream, const FullGlobalDiagnostic& d) {
	ostream << "FullGlobalDiagnostic{motionOfCOM: " << d.motionOfCOM << "\n";
	ostream << "getVelocity: " << d.getVelocity << "\n";
	ostream << "kineticEnergy: " << d.kineticEnergy << "\n";
	ostream << "inertiaInDirection: " << d.inertiaInDirection << "\n";
	ostream << "getTotalAngularMomentum: " << d.getTotalAngularMomentum << "\n";
	ostream << "lastPosition: " << d.positions[TICKS - 1] << "}";
	return ostream;
}

static Vec3 inertiaOfRelativePos = Vec3(2.5, 1.2, -2.0);
static Vec3 inertiaOfRelativeDir = normalize(Vec3(3.5, -2.0, 1.34));

static FullGlobalDiagnostic doDiagnostic(MotorizedPhysical* p) {
	FullGlobalDiagnostic result;

	result.motionOfCOM = p->getMotionOfCenterOfMass();
	result.getVelocity = p->getVelocityOfCenterOfMass();
	result.kineticEnergy = p->getKineticEnergy();
	result.inertiaInDirection = p->getInertiaOfPointInDirectionRelative(inertiaOfRelativePos, inertiaOfRelativeDir);
	result.getTotalAngularMomentum = p->getTotalAngularMomentum();

	for(std::size_t i = 0; i < TICKS; i++) {
		p->update(DELTA_T);
		result.positions[i] = p->getCenterOfMass();
	}

	return result;
}

static const Motion motionOfCOM = Motion(Vec3(1.3, 0.7, -2.1), Vec3(2.1, 0.7, 3.7));

static FullGlobalDiagnostic runDiagnosticForCFrame(MotorizedPhysical* p, const GlobalCFrame& cframeOfStart) {
	p->setCFrame(cframeOfStart);
	p->motionOfCenterOfMass = motionOfCOM;

	for(ConnectedPhysical& c : p->childPhysicals) {
		SinusoidalPistonConstraint* constraint = dynamic_cast<SinusoidalPistonConstraint*>(c.connectionToParent.constraintWithParent.get());
		constraint->currentStepInPeriod = 0;
	}

	p->fullRefreshOfConnectedPhysicals();
	p->refreshPhysicalProperties();

	return doDiagnostic(p);
}

TEST_CASE(basicFullRotationSymmetryInvariance) {
	const int ticksToSim = 10;

	Position origin(-143.3, 700.3, 1000.0);
	Part centerPart(sphereShape(1.0), GlobalCFrame(origin), basicProperties);
	Shape box = boxShape(1.0, 1.0, 1.0);

	Part xPart(box, centerPart, CFrame(1.0, 0.0, 0.0), basicProperties);
	Part yPart(box, centerPart, CFrame(0.0, 1.0, 0.0), basicProperties);
	Part zPart(box, centerPart, CFrame(0.0, 0.0, 1.0), basicProperties);
	Part nxPart(box, centerPart, CFrame(-1.0, 0.0, 0.0), basicProperties);
	Part nyPart(box, centerPart, CFrame(0.0, -1.0, 0.0), basicProperties);
	Part nzPart(box, centerPart, CFrame(0.0, 0.0, -1.0), basicProperties);

	MotorizedPhysical* motorPhys = centerPart.parent->mainPhysical;

	FullGlobalDiagnostic reference = runDiagnosticForCFrame(motorPhys, GlobalCFrame(origin));
	logStream << reference;

	Rotation rotations[9]{
		Rotation::Predefined::X_90,
		Rotation::Predefined::X_180,
		Rotation::Predefined::X_270,
		Rotation::Predefined::Y_90,
		Rotation::Predefined::Y_180,
		Rotation::Predefined::Y_270,
		Rotation::Predefined::Z_90,
		Rotation::Predefined::Z_180,
		Rotation::Predefined::Z_270,
	};
	for(int i = 0; i < 9; i++) {
		ASSERT(reference == runDiagnosticForCFrame(motorPhys, GlobalCFrame(origin, rotations[i])));
	}
}

TEST_CASE(angularMomentumOverLocalToGlobal) {
	const int ticksToSim = 10;

	Position origin(-143.3, 700.3, 1000.0);
	
	std::vector<Part> phys = produceMotorizedPhysical();
	MotorizedPhysical* motorPhys = phys[0].parent->mainPhysical;


	Motion motionOfCOM = Motion(Vec3(1.3, 0.7, -2.1), Vec3(2.1, 0.7, 3.7));
	Rotation rotation = Rotation::fromEulerAngles(1.0, 0.2, -0.9);

	motorPhys->setCFrame(GlobalCFrame(origin));
	motorPhys->motionOfCenterOfMass = motionOfCOM;

	Vec3 firstAngularMomentumFromParts = getTotalAngularMomentumOfPhysical(motorPhys);
	Vec3 firstAngularMomentum = motorPhys->getTotalAngularMomentum();

	motorPhys->setCFrame(GlobalCFrame(origin, rotation));
	motorPhys->motionOfCenterOfMass = localToGlobal(rotation, motionOfCOM);

	Vec3 secondAngularMomentumFromParts = getTotalAngularMomentumOfPhysical(motorPhys);
	Vec3 secondAngularMomentum = motorPhys->getTotalAngularMomentum();

	ASSERT(rotation.localToGlobal(firstAngularMomentum) == secondAngularMomentum);
	ASSERT(rotation.localToGlobal(firstAngularMomentumFromParts) == secondAngularMomentumFromParts);
}

TEST_CASE(hardConstrainedFullRotationFollowsCorrectly) {
	const int ticksToSim = 10;

	Position origin(-143.3, 700.3, 1000.0);
	Part centerPart(sphereShape(1.0), GlobalCFrame(origin), basicProperties);
	Shape box = boxShape(1.0, 1.0, 1.0);

	Part zPart(box, centerPart, new SinusoidalPistonConstraint(1.0, 3.0, 1.0), CFrame(0.0, 0.0, 0.0, Rotation::Predefined::IDENTITY), CFrame(0.0, 0.0, 0.0), basicProperties);
	Part yPart(box, centerPart, new SinusoidalPistonConstraint(1.0, 3.0, 1.0), CFrame(0.0, 0.0, 0.0, Rotation::Predefined::X_90), CFrame(0.0, 0.0, 0.0), basicProperties);
	Part xPart(box, centerPart, new SinusoidalPistonConstraint(1.0, 3.0, 1.0), CFrame(0.0, 0.0, 0.0, Rotation::Predefined::Y_90), CFrame(0.0, 0.0, 0.0), basicProperties);
	Part nzPart(box, centerPart, new SinusoidalPistonConstraint(1.0, 3.0, 1.0), CFrame(0.0, 0.0, 0.0, Rotation::Predefined::X_180), CFrame(0.0, 0.0, 0.0), basicProperties);
	Part nyPart(box, centerPart, new SinusoidalPistonConstraint(1.0, 3.0, 1.0), CFrame(0.0, 0.0, 0.0, Rotation::Predefined::X_270), CFrame(0.0, 0.0, 0.0), basicProperties);
	Part nxPart(box, centerPart, new SinusoidalPistonConstraint(1.0, 3.0, 1.0), CFrame(0.0, 0.0, 0.0, Rotation::Predefined::Y_270), CFrame(0.0, 0.0, 0.0), basicProperties);

	MotorizedPhysical* p = centerPart.parent->mainPhysical;

	FullGlobalDiagnostic reference = runDiagnosticForCFrame(p, GlobalCFrame(origin));
	logStream << reference;

	Rotation rotations[9]{
		Rotation::Predefined::X_90,
		Rotation::Predefined::X_180,
		Rotation::Predefined::X_270,
		Rotation::Predefined::Y_90,
		Rotation::Predefined::Y_180,
		Rotation::Predefined::Y_270,
		Rotation::Predefined::Z_90,
		Rotation::Predefined::Z_180,
		Rotation::Predefined::Z_270,
	};
	for(int i = 0; i < 9; i++) {
		ASSERT(reference == runDiagnosticForCFrame(p, GlobalCFrame(origin, rotations[i])));
	}
}

TEST_CASE(basicAngularMomentumOfSinglePart) {
	Polyhedron testPoly = Library::createPointyPrism(4, 1.0f, 1.0f, 0.5f, 0.5f);
	Part mainPart(polyhedronShape(testPoly), GlobalCFrame(Position(1.3, 2.7, -2.6), Rotation::fromEulerAngles(0.6, -0.7, -0.3)), basicProperties);

	mainPart.ensureHasParent();

	MotorizedPhysical* motorPhys = mainPart.parent->mainPhysical;

	motorPhys->motionOfCenterOfMass = Motion(Vec3(2.0, 3.0, 1.0), Vec3(-1.7, 3.3, 12.0));

	ALLOCA_COMMotionTree(t, mainPart.parent->mainPhysical, size);

	ASSERT(motorPhys->getTotalAngularMomentum() == getTotalAngularMomentumOfPhysical(motorPhys));
}

TEST_CASE(motorizedPhysicalAngularMomentum) {
	std::vector<Part> phys = produceMotorizedPhysical();
	MotorizedPhysical* motorPhys = phys[0].parent->mainPhysical;

	ALLOCA_COMMotionTree(t, motorPhys, size);

	ASSERT(t.getInternalAngularMomentum() == getTotalAngularMomentumOfPhysical(motorPhys));
	ASSERT(motorPhys->getTotalAngularMomentum() == getTotalAngularMomentumOfPhysical(motorPhys));
}

TEST_CASE(physicalTotalAngularMomentum) {
	std::vector<Part> phys = producePhysical();
	MotorizedPhysical* motorPhys = phys[0].parent->mainPhysical;

	motorPhys->motionOfCenterOfMass = Motion(Vec3(2.0, 3.0, 1.0), Vec3(-1.7, 3.3, 12.0));

	ALLOCA_COMMotionTree(t, motorPhys, size);

	ASSERT(motorPhys->getTotalAngularMomentum() == getTotalAngularMomentumOfPhysical(motorPhys));
}

TEST_CASE(basicMotorizedPhysicalTotalAngularMomentum) {
	std::vector<Part> result;
	result.reserve(10);
	Part& mainPart = result.emplace_back(boxShape(1.0, 1.0, 1.0), GlobalCFrame(), basicProperties);

	mainPart.ensureHasParent();

	Part& part1_mainPart = result.emplace_back(polyhedronShape(Library::house), mainPart,
											   new SinusoidalPistonConstraint(0.0, 2.0, 1.0),
											   CFrame(0.0, 0.0, 0.0),
											   CFrame(0.0, 0.0, 0.0), basicProperties);
	/*Part& part2_mainPart = result.emplace_back(polyhedronShape(Library::house), mainPart,
											   new MotorConstraintTemplate<ConstantMotorTurner>(1.7),
											   CFrame(-0.3, 0.7, 0.5, Rotation::fromEulerAngles(0.7, 0.3, 0.7)),
											   CFrame(0.7, -2.0, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)), basicProperties);*/
	/*Part& part3_mainPart = result.emplace_back(boxShape(1.0, 1.0, 1.0), mainPart,
											   new MotorConstraintTemplate<ConstantMotorTurner>(1.7),
											   CFrame(-0.3, 0.7, 0.5, Rotation::fromEulerAngles(0.7, 0.3, 0.7)),
											   CFrame(0.7, -2.0, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)), basicProperties);*/

	MotorizedPhysical* motorPhys = result[0].parent->mainPhysical;

	motorPhys->motionOfCenterOfMass = Motion(Vec3(0.0, 0.0, 0.0), Vec3(-1.7, 3.3, 12.0));

	ALLOCA_COMMotionTree(t, motorPhys, size);

	ASSERT(motorPhys->getTotalAngularMomentum() == getTotalAngularMomentumOfPhysical(motorPhys));
}

TEST_CASE(totalInertiaOfPhysical) {
	std::vector<Part> phys = producePhysical();
	MotorizedPhysical* motorPhys = phys[0].parent->mainPhysical;

	ALLOCA_COMMotionTree(t, motorPhys, size);

	ASSERT(t.getInertia() == getTotalInertiaOfPhysical(motorPhys));
}

TEST_CASE(totalInertiaOfBasicMotorizedPhysical) {
	std::vector<Part> result;
	result.reserve(10);
	Part& mainPart = result.emplace_back(polyhedronShape(Library::house), GlobalCFrame(), basicProperties);

	mainPart.ensureHasParent();

	/*Part& part1_mainPart = result.emplace_back(polyhedronShape(Library::house), mainPart,
											   new SinusoidalPistonConstraint(0.0, 2.0, 1.0),
											   CFrame(0.0, 0.0, 0.0),
											   CFrame(0.0, 0.0, 0.0), basicProperties);*/
	Part& part2_mainPart = result.emplace_back(polyhedronShape(Library::house), mainPart,
											   new MotorConstraintTemplate<ConstantMotorTurner>(1.7),
											   CFrame(-0.3, 0.7, 0.5, Rotation::fromEulerAngles(0.7, 0.3, 0.7)),
											   CFrame(0.7, -2.0, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)), basicProperties);
	/*Part& part3_mainPart = result.emplace_back(boxShape(1.0, 1.0, 1.0), mainPart,
												new MotorConstraintTemplate<ConstantMotorTurner>(1.7),
												CFrame(-0.3, 0.7, 0.5, Rotation::fromEulerAngles(0.7, 0.3, 0.7)),
												CFrame(0.7, -2.0, -0.5, Rotation::fromEulerAngles(0.2, -0.257, 0.4)), basicProperties);*/

	MotorizedPhysical* motorPhys = result[0].parent->mainPhysical;

	ALLOCA_COMMotionTree(t, motorPhys, size);

	ASSERT(t.getInertia() == getTotalInertiaOfPhysical(motorPhys));
}

TEST_CASE(totalCenterOfMassOfPhysical) {
	std::vector<Part> phys = producePhysical();
	MotorizedPhysical* motorPhys = phys[0].parent->mainPhysical;

	ASSERT(motorPhys->getCenterOfMass() == getTotalCenterOfMassOfPhysical(motorPhys));
}

TEST_CASE(totalCenterOfMassOfMotorizedPhysical) {
	std::vector<Part> phys = produceMotorizedPhysical();
	MotorizedPhysical* motorPhys = phys[0].parent->mainPhysical;

	ASSERT(motorPhys->getCenterOfMass() == getTotalCenterOfMassOfPhysical(motorPhys));
}

TEST_CASE(totalVelocityOfCenterOfMassOfPhysical) {
	std::vector<Part> phys = producePhysical();
	MotorizedPhysical* motorPhys = phys[0].parent->mainPhysical;

	motorPhys->motionOfCenterOfMass = motionOfCOM;

	ASSERT(motorPhys->getMotionOfCenterOfMass().getVelocity() == getTotalMotionOfCenterOfMassOfPhysical(motorPhys));
}

TEST_CASE(totalVelocityOfCenterOfMassOfMotorizedPhysical) {
	std::vector<Part> phys = produceMotorizedPhysical();
	MotorizedPhysical* motorPhys = phys[0].parent->mainPhysical;

	motorPhys->motionOfCenterOfMass = motionOfCOM;

	ASSERT(motorPhys->getMotionOfCenterOfMass().getVelocity() == getTotalMotionOfCenterOfMassOfPhysical(motorPhys));
}

TEST_CASE(totalInertiaOfMotorizedPhysical) {
	std::vector<Part> phys = produceMotorizedPhysical();
	MotorizedPhysical* motorPhys = phys[0].parent->mainPhysical;

	ALLOCA_COMMotionTree(t, motorPhys, size);

	ASSERT(t.getInertia() == getTotalInertiaOfPhysical(motorPhys));
}

TEST_CASE(motorizedPhysicalTotalAngularMomentum) {
	std::vector<Part> phys = produceMotorizedPhysical();
	MotorizedPhysical* motorPhys = phys[0].parent->mainPhysical;

	motorPhys->motionOfCenterOfMass = Motion(Vec3(2.0, 3.0, 1.0), Vec3(-1.7, 3.3, 12.0));

	ALLOCA_COMMotionTree(t, motorPhys, size);

	ASSERT(motorPhys->getTotalAngularMomentum() == getTotalAngularMomentumOfPhysical(motorPhys));
}

TEST_CASE(conservationOfAngularMomentum) {
	std::vector<Part> phys = produceMotorizedPhysical();

	MotorizedPhysical* motorPhys = phys[0].parent->mainPhysical;

	motorPhys->motionOfCenterOfMass = Motion(Vec3(2.0, 3.0, 1.0), Vec3(-1.7, 3.3, 12.0));

	Vec3 initialAngularMomentum = motorPhys->getTotalAngularMomentum();

	for(int i = 0; i < TICKS; i++) {
		motorPhys->update(DELTA_T);

		ASSERT(initialAngularMomentum == motorPhys->getTotalAngularMomentum());
	}
}

TEST_CASE(conservationOfCenterOfMass) {
	std::vector<Part> phys = produceMotorizedPhysical();

	MotorizedPhysical* motorPhys = phys[0].parent->mainPhysical;

	motorPhys->motionOfCenterOfMass = Motion(Vec3(0.0, 0.0, 0.0), Vec3(-1.7, 3.3, 12.0));

	Position initialCenterOfMass = motorPhys->getCenterOfMass();

	for(int i = 0; i < TICKS; i++) {
		motorPhys->update(DELTA_T);

		ASSERT(initialCenterOfMass == motorPhys->getCenterOfMass());
	}
}

TEST_CASE(angularMomentumVelocityInvariance) {
	std::vector<Part> phys = produceMotorizedPhysical();

	MotorizedPhysical* motorPhys = phys[0].parent->mainPhysical;

	motorPhys->motionOfCenterOfMass = Motion(Vec3(0.0, 0.0, 0.0), Vec3(-1.7, 3.3, 12.0));

	Vec3 stillAngularMomentum = motorPhys->getTotalAngularMomentum();
	Vec3 stillAngularMomentumPartsBased = getTotalAngularMomentumOfPhysical(motorPhys);

	motorPhys->motionOfCenterOfMass = Motion(Vec3(50.3, 12.3, -74.2), Vec3(-1.7, 3.3, 12.0));

	Vec3 movingAngularMomentum = motorPhys->getTotalAngularMomentum();
	Vec3 movingAngularMomentumPartsBased = getTotalAngularMomentumOfPhysical(motorPhys);

	ASSERT(stillAngularMomentum == movingAngularMomentum);
	ASSERT(stillAngularMomentumPartsBased == movingAngularMomentumPartsBased);
}
