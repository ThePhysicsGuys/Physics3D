#include "testsMain.h"

#include "../engine/world.h"
#include "../application/objectLibrary.h"
#include "../engine/math/mathUtil.h"


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
