#include "testsMain.h"

#include "../engine/world.h"
#include "../application/objectLibrary.h"
#include "../application/worlds.h"
#include "../engine/math/mathUtil.h"


#define REMAINS_CONSTANT(v) REMAINS_CONSTANT_TOLERANT(v, 0.00005)
#define ASSERT(v) ASSERT_TOLERANT(v, 0.00005)

double PI = 3.14159265359;

static const double DELTA_T = 0.01;
static const int TICKS = 500;

TEST_CASE(positionInvariance) {
	Mat3 rotation = fromEulerAngles(0, 0, 0);

	Vec3 origins[]{Vec3(0,0,0), Vec3(0,0,1), Vec3(5.3,0,-2.4),Vec3(0.3,0,0.7),Vec3(0.0000001,0,0.00000001)};

	Part housePart(house, 1.0, 1.0);
	Part icosaPart(icosahedron, 10.0, 0.7);

	CFrame houseRelative(Vec3(0.7, 0.6, 1.6), fromEulerAngles(0.3, 0.7, 0.9));
	CFrame icosaRelative(Vec3(0.7, 3.0, 1.6), fromEulerAngles(0.1, 0.1, 0.1));

	for(Vec3 o:origins) {
		CFrame origin(o, rotation);

		Physical housePhysical(housePart, origin.localToGlobal(houseRelative));
		Physical icosaPhysical(icosaPart, origin.localToGlobal(icosaRelative));

		GravityFloorWorld w(Vec3(0.0, -10.0, 0.0));

		w.addObject(housePhysical);
		w.addObject(icosaPhysical);

		for(int i = 0; i < TICKS; i++)
			w.tick(DELTA_T);

		REMAINS_CONSTANT_TOLERANT(origin.globalToLocal(w.physicals[0].cframe), 0.00001);
		REMAINS_CONSTANT_TOLERANT(origin.globalToLocal(w.physicals[1].cframe), 0.00001);
	}
}

TEST_CASE(rotationInvariance) {
	//Mat3 origins[]{fromEulerAngles(0,0,0), fromEulerAngles(0,1,0), fromEulerAngles(0,0.5,0), fromEulerAngles(0,-0.5,0),};

	Part housePart(house, 1.0, 1.0);
	Part icosaPart(icosahedron, 10.0, 0.7);

	CFrame houseRelative(Vec3(0.7, 0.6, 1.6), fromEulerAngles(0.3, 0.7, 0.9));
	CFrame icosaRelative(Vec3(0.7, 3.0, 1.6), fromEulerAngles(0.1, 0.1, 0.1));

	for(double rotation = -1.5; rotation < 1.500001; rotation += 0.1) {
		logStream << rotation << '\n';

		CFrame origin(Vec3(0,0,0), fromEulerAngles(0.0, rotation, 0.0));

		Physical housePhysical(housePart, origin.localToGlobal(houseRelative));
		Physical icosaPhysical(icosaPart, origin.localToGlobal(icosaRelative));

		GravityFloorWorld w(Vec3(0.0, -10.0, 0.0));

		w.addObject(housePhysical);
		w.addObject(icosaPhysical);

		for(int i = 0; i < TICKS; i++)
			w.tick(DELTA_T);

		REMAINS_CONSTANT_TOLERANT(origin.globalToLocal(w.physicals[0].cframe), 0.00001);
		REMAINS_CONSTANT_TOLERANT(origin.globalToLocal(w.physicals[1].cframe), 0.00001);
	}
}

TEST_CASE(applyForceToRotate) {
	Physical p = Physical(Part(createCube(1.0), 1.0, 1.0), CFrame(fromEulerAngles(0.3, 0.7, 0.9)));

	Vec3 relAttach = Vec3(1.0, 0.0, 0.0);
	Vec3 force = Vec3(0.0, 1.0, 0.0);

	p.applyForce(relAttach, force);
	ASSERT(p.totalForce == force);
	ASSERT(p.totalMoment == Vec3(0.0, 0.0, 1.0));
}

TEST_CASE(momentToAngularVelocity) {
	Physical p = Physical(Part(createCube(1.0), 1.0, 1.0), CFrame(rotY(PI/2)));

	Vec3 moment(1.0, 0.0, 0.0);

	for(int i = 0; i < 50; i++) {
		p.applyMoment(moment);
		p.update(0.05);
	}

	ASSERT(p.angularVelocity == moment * 50 * 0.05 / p.inertia.m00);
}