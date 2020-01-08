#include "testsMain.h"

#include "compare.h"
#include "../physics/misc/toString.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "../physics/world.h"
#include "../physics/inertia.h"
#include "../physics/misc/shapeLibrary.h"
#include "../physics/math/linalg/trigonometry.h"
#include "../physics/math/linalg/misc.h"
#include "../physics/math/linalg/commonMatrices.h"
#include "../physics/math/linalg/eigen.h"
#include "../physics/geometry/basicShapes.h"
#include "../physics/geometry/shape.h"
#include "../physics/geometry/polyhedron.h"
#include "../physics/geometry/normalizedPolyhedron.h"
#include "../util/log.h"


#define REMAINS_CONSTANT(v) REMAINS_CONSTANT_TOLERANT(v, 0.0005)
#define ASSERT(v) ASSERT_TOLERANT(v, 0.0005)

static const double DELTA_T = 0.01;
static const int TICKS = 500;
/*
TEST_CASE(positionInvariance) {
	Mat3 rotation = fromEulerAngles(0.0, 0.0, 0.0);

	Position origins[]{Position(0,0,0), Position(0,0,1), Position(5.3,0,-2.4),Position(0.3,0,0.7),Position(0.0000001,0,0.00000001)};

	CFrame houseRelative(Vec3(0.7, 0.6, 1.6), fromEulerAngles(0.3, 0.7, 0.9));
	CFrame icosaRelative(Vec3(0.7, 3.0, 1.6), fromEulerAngles(0.1, 0.1, 0.1));

	for(Position o:origins) {
		GlobalCFrame origin(o, rotation);

		Part housePart(Library::house, origin.localToGlobal(houseRelative), {1.0, 1.0, 0.7});
		Part icosaPart(Library::icosahedron, origin.localToGlobal(icosaRelative), {10.0, 0.7, 0.7});

		World<Part> world(DELTA_T);

		world.addPart(&housePart);
		world.addPart(&icosaPart);

		for(int i = 0; i < TICKS; i++)
			world.tick();

		REMAINS_CONSTANT(origin.globalToLocal(housePart.getCFrame()));
		REMAINS_CONSTANT(origin.globalToLocal(icosaPart.getCFrame()));
	}
}*/
/*
TEST_CASE(rotationInvariance) {
	//Mat3 origins[]{fromEulerAngles(0,0,0), fromEulerAngles(0,1,0), fromEulerAngles(0,0.5,0), fromEulerAngles(0,-0.5,0),};

	

	CFrame houseRelative(Vec3(0.7, 0.6, 1.6), fromEulerAngles(0.3, 0.7, 0.9));
	CFrame icosaRelative(Vec3(0.7, 3.0, 1.6), fromEulerAngles(0.1, 0.1, 0.1));

	for(double rotation = -1.5; rotation < 1.500001; rotation += 0.1) {
		logStream << rotation << '\n';

		GlobalCFrame origin(0,0,0, rotY(rotation));

		Part housePart(Library::house, origin.localToGlobal(houseRelative), {1.0, 1.0, 0.7});
		Part icosaPart(Library::icosahedron, origin.localToGlobal(icosaRelative), {10.0, 0.7, 0.7});

		World<> w(DELTA_T);

		w.addPart(&housePart);
		w.addPart(&icosaPart);

		for(int i = 0; i < TICKS; i++)
			w.tick();

		REMAINS_CONSTANT(origin.globalToLocal(housePart.getCFrame()));
		REMAINS_CONSTANT(origin.globalToLocal(icosaPart.getCFrame()));
	}
}*/

TEST_CASE(applyForceToRotate) {
	Part part(Box(1.0, 1.0, 1.0), GlobalCFrame(0,0,0,fromEulerAngles(0.3, 0.7, 0.9)), {1.0, 1.0, 0.7});
	part.ensureHasParent();

	Vec3 relAttach = Vec3(1.0, 0.0, 0.0);
	Vec3 force = Vec3(0.0, 1.0, 0.0);

	part.parent->mainPhysical->applyForce(relAttach, force);
	ASSERT(part.parent->mainPhysical->totalForce == force);
	ASSERT(part.parent->mainPhysical->totalMoment == Vec3(0.0, 0.0, 1.0));
}

TEST_CASE(momentToAngularVelocity) {
	Part part(Box(1.0, 1.0, 1.0), GlobalCFrame(rotY(M_PI / 2)), {1.0, 1.0, 0.7});
	part.ensureHasParent();
	MotorizedPhysical& p = *part.parent->mainPhysical;

	Vec3 moment(1.0, 0.0, 0.0);

	for(int i = 0; i < 50; i++) {
		p.applyMoment(moment);
		p.update(0.05);
	}

	ASSERT(p.getMotion().angularVelocity == moment * 50 * 0.05 * p.momentResponse[0][0]);
}

TEST_CASE(rotationImpulse) {
	Part part(Box(0.2, 20.0, 0.2), GlobalCFrame(0,0,0), {1.0, 1.0, 0.7});
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

TEST_CASE(testPointAcceleration) {
	Part testPart(Box(1.0, 2.0, 3.0), GlobalCFrame(0,0,0), {1.0, 1.0, 0.7});
	testPart.ensureHasParent();
	MotorizedPhysical& testPhys = *testPart.parent->mainPhysical;
	Vec3 localPoint(3, 5, 7);
	Vec3 force(-4, -3, 0.5);
	double deltaT = 0.00001;

	testPhys.applyForce(localPoint, force);

	Vec3 acceleration = testPhys.getMotion().acceleration;
	Vec3 angularAcceleration = testPhys.getMotion().angularAcceleration;
	Vec3 pointAcceleration = testPhys.getMotion().getAccelerationOfPoint(localPoint);

	testPhys.update(deltaT);

	Vec3 actualAcceleration = testPhys.getMotion().velocity / deltaT;
	Vec3 actualAngularAcceleration = testPhys.getMotion().angularVelocity / deltaT;
	Vec3 actualPointAcceleration = testPhys.getMotion().getVelocityOfPoint(testPhys.getCFrame().localToRelative(localPoint)) / deltaT;

	ASSERT(acceleration == actualAcceleration);
	ASSERT(angularAcceleration == actualAngularAcceleration);
	ASSERT(pointAcceleration == actualPointAcceleration);
}

TEST_CASE(testGetPointAccelerationMatrix) {
	Part testPart(Box(1.0, 2.0, 3.0), GlobalCFrame(0,0,0), {1.0, 1.0, 0.7});
	testPart.ensureHasParent();
	MotorizedPhysical& testPhys = *testPart.parent->mainPhysical;
	Vec3 localPoint(3, 5, 7);
	Vec3 force(-4, -3, 0.5);

	testPhys.applyForce(localPoint, force);

	SymmetricMat3 accelMatrix = testPhys.getResponseMatrix(localPoint);

	logStream << accelMatrix;

	Vec3 actualAcceleration = testPhys.getMotion().getAccelerationOfPoint(localPoint);

	ASSERT(actualAcceleration == accelMatrix * force);
}
TEST_CASE(impulseTest) {
	Part part(Box(1.0, 2.0, 2.5), GlobalCFrame(0,0,0), {1.0, 1.0, 0.7});
	part.ensureHasParent();
	MotorizedPhysical& p = *part.parent->mainPhysical;

	p.applyImpulseAtCenterOfMass(Vec3(15, 0, 0));
	ASSERT(p.getMotion().velocity == Vec3(3,0,0));
	ASSERT(p.getMotion().angularVelocity == Vec3(0, 0, 0));

	Vec3 angularImpulse = Vec3(0, 2, 0) % Vec3(-15, 0, 0);

	p.applyImpulse(Vec3(0, 2, 0), Vec3(-15, 0, 0));
	ASSERT(p.getMotion().velocity == Vec3(0, 0, 0));
	ASSERT(p.getMotion().angularVelocity == ~part.getInertia() * angularImpulse);
}

TEST_CASE(testPointAccelMatrixImpulse) {
	Part part(Box(1.0, 2.0, 3.0), GlobalCFrame(7.6, 3.4, 3.9, fromEulerAngles(1.1, 0.7, 0.9)), {1.0, 1.0, 0.7});
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
	Part part(Box(1.0, 2.0, 3.0), GlobalCFrame(7.6, 3.4, 3.9, fromEulerAngles(1.1, 0.7, 0.9)), {1.0, 1.0, 0.7});
	part.ensureHasParent();
	MotorizedPhysical& p = *part.parent->mainPhysical;

	Vec3 localPoint(0.8, 0.6, 0.9);
	Vec3 relativePoint = p.getCFrame().localToRelative(localPoint);

	p.getMotion().velocity = Vec3(0.3, -1.3, 1.2);
	p.getMotion().angularVelocity = Vec3(0.7, 0.5, -0.9);

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
	Part part(Box(1.0, 2.0, 3.0), GlobalCFrame(/*Vec3(7.6, 3.4, 3.9), fromEulerAngles(1.1, 0.7, 0.9)*/), {1.0, 1.0, 0.7});
	part.ensureHasParent();
	MotorizedPhysical& p = *part.parent->mainPhysical;

	Vec3 localPoint(0.8, 0.6, 0.9);
	Vec3 relativePoint = p.getCFrame().localToRelative(localPoint);
	Vec3 normal(0.0, 170.0, 0.0);

	p.getMotion().velocity = Vec3(0.3, -1.3, 1.2);
	p.getMotion().angularVelocity = Vec3(0.7, 0.5, -0.9);

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
	Part part(Polyhedron(), CFrame(Vec3(7.6, 3.4, 3.9), fromEulerAngles(1.1, 0.7, 0.9)), {1.0, 1.0, 0.7});
	MotorizedPhysical p(&part, 5.0, DiagonalMat3(2, 7, 5));

	Vec3 localPoint(0.8, 0.6, 0.9);
	Vec3 localImpulse(0.3, -0.7, 0.6);

	Vec3 estimatedAccel = p.getResponseMatrix(localPoint) * localImpulse;

	p.applyImpulse(part.getCFrame().localToRelative(localPoint), part.getCFrame().localToRelative(localImpulse));

	Vec3 realAccel = part.getCFrame().relativeToLocal(p.getVelocityOfPoint(part.getCFrame().localToRelative(localPoint)));

	ASSERT(estimatedAccel == realAccel);
}*/

TEST_CASE(testChangeInertialBasis) {
	RotMat3 rotation = fromEulerAngles(0.6, 0.3, 0.7);
	Polyhedron rotatedTriangle = Library::trianglePyramid.rotated(rotation);
	SymmetricMat3 triangleInertia = Library::trianglePyramid.getInertia(CFrame());
	SymmetricMat3 rotatedTriangleInertia = rotatedTriangle.getInertia(CFrame());

	ASSERT(getEigenDecomposition(triangleInertia).eigenValues == getEigenDecomposition(rotatedTriangleInertia).eigenValues);
	ASSERT(getRotatedInertia(triangleInertia, rotation) == rotatedTriangleInertia);
}

TEST_CASE(movedInertialMatrixForBox) {
	Polyhedron originalShape = Library::createBox(1.0, 2.0, 3.0);
	Vec3 translation(3.1, -2.7, 7.9);
	Polyhedron translatedTriangle = originalShape.translated(translation);
	SymmetricMat3 triangleInertia = originalShape.getInertia(CFrame());
	SymmetricMat3 translatedTriangleInertia = translatedTriangle.getInertia(CFrame());

	ASSERT(getTranslatedInertia(triangleInertia, translation, originalShape.getCenterOfMass(), originalShape.getVolume()) == translatedTriangleInertia);
}

TEST_CASE(movedInertialMatrixForDifficuiltPart) {
	Polyhedron originalShape = Library::trianglePyramid;
	Vec3 translation(3.1, -2.7, 7.9);
	Polyhedron translatedTriangle = originalShape.translated(translation);
	SymmetricMat3 triangleInertia = originalShape.getInertia(CFrame());
	SymmetricMat3 translatedTriangleInertia = translatedTriangle.getInertia(CFrame());

	ASSERT(getTranslatedInertia(triangleInertia, translation, originalShape.getCenterOfMass(), originalShape.getVolume()) == translatedTriangleInertia);
}

TEST_CASE(testMultiPartPhysicalSimple) {
	Shape box(Box(1.0, 0.5, 0.5));
	Shape box2(Box(1.0, 0.5, 0.5));
	Shape doubleBox(Box(2.0, 0.5, 0.5));
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
	Shape box(Box(1.0, 0.5, 0.5));
	Shape box2(Box(0.5, 0.5, 1.0));
	Shape doubleBox(Box(2.0, 0.5, 0.5));
	Part* p1 = new Part(box, GlobalCFrame(), {10.0, 0.0, 0.7});
	Part* p2 = new Part(box2, GlobalCFrame(), {10.0, 0.0, 0.7});
	Part* doubleP = new Part(doubleBox, GlobalCFrame(), {10.0, 0, 0.7});

	MotorizedPhysical phys(p1);
	phys.attachPart(p2, CFrame(Vec3(1.0, 0.0, 0.0), ROT_Y_90(double)));

	MotorizedPhysical phys2(doubleP);

	ASSERT(phys.totalMass == p1->getMass() + p2->getMass());
	ASSERT(phys.totalCenterOfMass == Vec3(0.5, 0, 0));
	ASSERT(phys.forceResponse == phys2.forceResponse);
	ASSERT(phys.momentResponse == phys2.momentResponse);
}

TEST_CASE(testShapeNativeScaling) {
	NormalizedPolyhedron testPoly = Library::trianglePyramid.normalized();

	Shape shape1(testPoly);
	Shape shape2 = shape1.scaled(2.0, 4.0, 3.7);

	Polyhedron scaledTestPoly = testPoly.scaled(2.0, 4.0, 3.7);

	ASSERT(shape2.getInertia() == scaledTestPoly.getInertiaAroundCenterOfMass());
}
