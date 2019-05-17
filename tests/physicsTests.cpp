#include "testsMain.h"

#include "../engine/world.h"
#include "../application/objectLibrary.h"
#include "../engine/math/mathUtil.h"
#include "../util/log.h"


#define REMAINS_CONSTANT(v) REMAINS_CONSTANT_TOLERANT(v, 0.0005)
#define ASSERT(v) ASSERT_TOLERANT(v, 0.0005)

double PI = 3.14159265359;

static const double DELTA_T = 0.01;
static const int TICKS = 500;

TEST_CASE(positionInvariance) {
	Mat3 rotation = fromEulerAngles(0.0, 0.0, 0.0);

	Vec3 origins[]{Vec3(0,0,0), Vec3(0,0,1), Vec3(5.3,0,-2.4),Vec3(0.3,0,0.7),Vec3(0.0000001,0,0.00000001)};

	CFrame houseRelative(Vec3(0.7, 0.6, 1.6), fromEulerAngles(0.3, 0.7, 0.9));
	CFrame icosaRelative(Vec3(0.7, 3.0, 1.6), fromEulerAngles(0.1, 0.1, 0.1));

	for(Vec3 o:origins) {
		CFrame origin(o, rotation);

		Part housePart(house, origin.localToGlobal(houseRelative), 1.0, 1.0);
		Part icosaPart(icosahedron, origin.localToGlobal(icosaRelative), 10.0, 0.7);

		World<> w;

		w.addObject(&housePart);
		w.addObject(&icosaPart);

		for(int i = 0; i < TICKS; i++)
			w.tick(DELTA_T);

		REMAINS_CONSTANT(origin.globalToLocal(w.parts[0]->cframe));
		REMAINS_CONSTANT(origin.globalToLocal(w.parts[1]->cframe));
	}
}

TEST_CASE(rotationInvariance) {
	//Mat3 origins[]{fromEulerAngles(0,0,0), fromEulerAngles(0,1,0), fromEulerAngles(0,0.5,0), fromEulerAngles(0,-0.5,0),};

	

	CFrame houseRelative(Vec3(0.7, 0.6, 1.6), fromEulerAngles(0.3, 0.7, 0.9));
	CFrame icosaRelative(Vec3(0.7, 3.0, 1.6), fromEulerAngles(0.1, 0.1, 0.1));

	for(double rotation = -1.5; rotation < 1.500001; rotation += 0.1) {
		logStream << rotation << '\n';

		CFrame origin(Vec3(0,0,0), rotY(rotation));

		Part housePart(house, origin.localToGlobal(houseRelative), 1.0, 1.0);
		Part icosaPart(icosahedron, origin.localToGlobal(icosaRelative), 10.0, 0.7);

		World<> w;

		w.addObject(&housePart);
		w.addObject(&icosaPart);

		for(int i = 0; i < TICKS; i++)
			w.tick(DELTA_T);

		REMAINS_CONSTANT(origin.globalToLocal(w.parts[0]->cframe));
		REMAINS_CONSTANT(origin.globalToLocal(w.parts[1]->cframe));
	}
}

TEST_CASE(applyForceToRotate) {
	Part part(createCube(1.0), CFrame(fromEulerAngles(0.3, 0.7, 0.9)), 1.0, 1.0);
	Physical p(&part);

	Vec3 relAttach = Vec3(1.0, 0.0, 0.0);
	Vec3 force = Vec3(0.0, 1.0, 0.0);

	p.applyForce(relAttach, force);
	ASSERT(p.totalForce == force);
	ASSERT(p.totalMoment == Vec3(0.0, 0.0, 1.0));
}

TEST_CASE(momentToAngularVelocity) {
	Part part(createCube(1.0), CFrame(rotY(PI / 2)), 1.0, 1.0);
	Physical p(&part);

	Vec3 moment(1.0, 0.0, 0.0);

	for(int i = 0; i < 50; i++) {
		p.applyMoment(moment);
		p.update(0.05);
	}

	ASSERT(p.angularVelocity == moment * 50 * 0.05 / p.inertia.m00);
}

TEST_CASE(rotationImpulse) {
	Part part(BoundingBox{-0.1, -10, -0.1, 0.1, 10, 0.1}.toShape(new Vec3[8]), CFrame(Vec3()), 1.0, 1.0);
	Physical veryLongBoxPhysical(&part);

	Vec3 xMoment = Vec3(1.0, 0.0, 0.0);
	Vec3 yMoment = Vec3(0.0, 1.0, 0.0);
	Vec3 zMoment = Vec3(0.0, 0.0, 1.0);

	{
		veryLongBoxPhysical.totalForce = Vec3();
		veryLongBoxPhysical.totalMoment = Vec3();
	}
}

TEST_CASE(testPointAcceleration) {
	Part testPart(NormalizedShape(), CFrame(), 1.0, 1.0);
	Physical testPhys(&testPart, 5.0, DiagonalMat3(1, 2, 3));
	Vec3 localPoint(3, 5, 7);
	Vec3 force(-4, -3, 0.5);
	double deltaT = 0.00001;

	testPhys.applyForce(localPoint, force);

	Vec3 acceleration = testPhys.getAcceleration();
	Vec3 angularAcceleration = testPhys.getAngularAcceleration();
	Vec3 pointAcceleration = testPhys.getAccelerationOfPoint(localPoint);

	testPhys.update(deltaT);

	Vec3 actualAcceleration = testPhys.velocity / deltaT;
	Vec3 actualAngularAcceleration = testPhys.angularVelocity / deltaT;
	Vec3 actualPointAcceleration = testPhys.getVelocityOfPoint(testPhys.getCFrame().localToRelative(localPoint)) / deltaT;

	ASSERT(acceleration == actualAcceleration);
	ASSERT(angularAcceleration == actualAngularAcceleration);
	ASSERT(pointAcceleration == actualPointAcceleration);
}

TEST_CASE(testGetPointAccelerationMatrix) {
	Part testPart(NormalizedShape(), CFrame(), 1.0, 1.0);
	Physical testPhys(&testPart, 5.0, DiagonalMat3(1, 2, 3));
	Vec3 localPoint(3, 5, 7);
	Vec3 force(-4, -3, 0.5);

	testPhys.applyForce(localPoint, force);

	SymmetricMat3 accelMatrix = testPhys.getPointAccelerationMatrix(localPoint);

	logStream << accelMatrix;

	Vec3 actualAcceleration = testPhys.getAccelerationOfPoint(localPoint);

	ASSERT(actualAcceleration == accelMatrix * force);
}
TEST_CASE(testComputeCombinedInertiaBetween) {
	Part testPart1(NormalizedShape(), CFrame(), 1.0, 1.0);
	Physical testPhys1(&testPart1, 5.0, DiagonalMat3(100, 200, 300));
	Part testPart2(NormalizedShape(), CFrame(Vec3(1.0, 0.0, 0.0)), 1.0, 1.0);
	Physical testPhys2(&testPart2, 5.0, DiagonalMat3(100, 200, 300));
	
	Vec3 direction(2.0, 0.0, 0.0);
	Vec3 colissionPos(0.5, 0.0, 0.0);

	double realInertiaBetweenNormalToCOM = 1 / (1 / testPhys1.mass + 1 / testPhys2.mass);
	double combinedInertiaBetweenNormalToCOM = computeCombinedInertiaBetween(testPhys1, testPhys2, colissionPos - testPhys1.getCenterOfMass(), colissionPos - testPhys2.getCenterOfMass(), direction);

	ASSERT(realInertiaBetweenNormalToCOM == combinedInertiaBetweenNormalToCOM);

	for(double x = -1.0; x <= 1.0; x += 0.25) {
		for(double y = -1.0; y <= 1.0; y += 0.25) {
			for(double z = -1.0; z <= 1.0; z += 0.25) {
				Vec3 colissionPos(x, y, z);
				double combinedInertiaBetweenNormal = computeCombinedInertiaBetween(testPhys1, testPhys2, colissionPos - testPhys1.getCenterOfMass(), colissionPos - testPhys2.getCenterOfMass(), direction);
				ASSERT(realInertiaBetweenNormalToCOM >= combinedInertiaBetweenNormalToCOM);
			}
		}
	}
}
TEST_CASE(impulseTest) {
	Part part(NormalizedShape(), CFrame(), 1.0, 1.0);
	Physical p(&part, 5.0, DiagonalMat3(5, 5, 5));

	p.applyImpulseAtCenterOfMass(Vec3(15, 0, 0));
	ASSERT(p.velocity == Vec3(3,0,0));
	ASSERT(p.angularVelocity == Vec3(0, 0, 0));

	p.applyImpulse(Vec3(0, 2, 0), Vec3(-15, 0, 0));
	ASSERT(p.velocity == Vec3(0, 0, 0));
	ASSERT(p.angularVelocity == Vec3(0, 0, 6));

	p.velocity = Vec3();
	p.angularVelocity = Vec3();


}

TEST_CASE(testPointAccelMatrixImpulse) {
	Part part(NormalizedShape(), CFrame(Vec3(7.6, 3.4, 3.9), fromEulerAngles(1.1, 0.7, 0.9)), 1.0, 1.0);
	Physical p(&part, 5.0, DiagonalMat3(2, 7, 5));

	Vec3 localPoint(0.8, 0.6, 0.9);
	Vec3 localImpulse(0.3, -0.7, 0.6);

	Vec3 estimatedAccel = p.getPointAccelerationMatrix(localPoint) * localImpulse;

	p.applyImpulse(part.cframe.localToRelative(localPoint), part.cframe.localToRelative(localImpulse));

	Vec3 realAccel = part.cframe.relativeToLocal(p.getVelocityOfPoint(part.cframe.localToRelative(localPoint)));

	ASSERT(estimatedAccel == realAccel);
}

TEST_CASE(inelasticColission) {
	Part part(NormalizedShape(), CFrame(Vec3(7.6, 3.4, 3.9), fromEulerAngles(1.1, 0.7, 0.9)), 1.0, 1.0);
	Physical p(&part, 5.0, DiagonalMat3(2, 7, 5));

	Vec3 localPoint(0.8, 0.6, 0.9);
	Vec3 relativePoint = p.getCFrame().localToRelative(localPoint);

	p.velocity = Vec3(0.3, -1.3, 1.2);
	p.angularVelocity = Vec3(0.7, 0.5, -0.9);

	Vec3 velOfPoint = p.getVelocityOfPoint(relativePoint);

	ASSERT(velOfPoint.y < 0);

	Log::warn("totalVelocity: " + str(velOfPoint));

	Vec3 direction(0.0, 170.0, 0.0);

	//double inertia = p.getInertiaOfPointInDirection(localPoint, p.getCFrame().relativeToLocal(direction));

	//Log::warn("inertia: %f", inertia);

	Log::warn("velInDirection: %f", velOfPoint * direction.normalize());

	//Vec3 relativeImpulse = -velOfPoint * direction.normalize() * direction.normalize() * inertia;

	Vec3 desiredAccel = -velOfPoint * direction * direction / direction.lengthSquared();
	Vec3 relativeImpulse = p.getCFrame().localToRelative(~p.getPointAccelerationMatrix(localPoint) * p.getCFrame().relativeToLocal(desiredAccel));
	Vec3 estimatedAccelLocal = p.getPointAccelerationMatrix(localPoint) * p.getCFrame().relativeToLocal(relativeImpulse);

	Vec3 estimatedAccelRelative = p.getCFrame().localToRelative(estimatedAccelLocal);
	

	p.applyImpulse(relativePoint, relativeImpulse);
	

	Vec3 velOfPointAfter = p.getVelocityOfPoint(relativePoint);
	Log::warn("New velocity: " + str(velOfPointAfter));
	Log::warn("velInDirection After: %f", velOfPointAfter * direction.normalize());
	Log::warn("estimatedAccelRelative: " + str(estimatedAccelRelative));
	Log::warn("Actual accel:           " + str(velOfPointAfter - velOfPoint));

	ASSERT(estimatedAccelRelative == velOfPointAfter - velOfPoint);
	ASSERT(velOfPointAfter.y == 0);
}

TEST_CASE(inelasticColission2) {
	Part part(NormalizedShape(), CFrame(/*Vec3(7.6, 3.4, 3.9), fromEulerAngles(1.1, 0.7, 0.9)*/), 1.0, 1.0);
	Physical p(&part, 5.0, DiagonalMat3(2, 7, 5));

	Vec3 localPoint(0.8, 0.6, 0.9);
	Vec3 relativePoint = p.getCFrame().localToRelative(localPoint);
	Vec3 normal(0.0, 170.0, 0.0);

	p.velocity = Vec3(0.3, -1.3, 1.2);
	p.angularVelocity = Vec3(0.7, 0.5, -0.9);

	Vec3 velOfPoint = p.getVelocityOfPoint(relativePoint);

	ASSERT(velOfPoint.y < 0);

	double inertia = p.getInertiaOfPointInDirection(localPoint, part.cframe.relativeToLocal(normal));

	double normalVelocity = velOfPoint * normal.normalize();

	double desiredAccel = -normalVelocity;

	Vec3 impulse = normal.normalize() * desiredAccel * inertia;



	p.applyImpulse(relativePoint, impulse);


	Vec3 velOfPointAfter = p.getVelocityOfPoint(relativePoint);
	Log::warn("New velocity: " + str(velOfPointAfter));
	Log::warn("velInDirection After: %f", velOfPointAfter * normal.normalize());
	//Log::warn("estimatedAccelRelative: " + str(estimatedAccelRelative));
	Log::warn("Actual accel:           " + str(velOfPointAfter - velOfPoint));

	//ASSERT(estimatedAccelRelative == velOfPointAfter - velOfPoint);
	ASSERT(velOfPointAfter.y == 0);
}

/*TEST_CASE(testPointAccelMatrixAndInertiaInDirection) {
	Part part(NormalizedShape(), CFrame(Vec3(7.6, 3.4, 3.9), fromEulerAngles(1.1, 0.7, 0.9)), 1.0, 1.0);
	Physical p(&part, 5.0, DiagonalMat3(2, 7, 5));

	Vec3 localPoint(0.8, 0.6, 0.9);
	Vec3 localImpulse(0.3, -0.7, 0.6);

	Vec3 estimatedAccel = p.getPointAccelerationMatrix(localPoint) * localImpulse;

	p.applyImpulse(part.cframe.localToRelative(localPoint), part.cframe.localToRelative(localImpulse));

	Vec3 realAccel = part.cframe.relativeToLocal(p.getVelocityOfPoint(part.cframe.localToRelative(localPoint)));

	ASSERT(estimatedAccel == realAccel);
}*/

TEST_CASE(testWorldImpulseProduction) {

}
