#include "testsMain.h"

#include "../engine/world.h"
#include "../application/objectLibrary.h"
#include "../application/worlds.h"
#include "../engine/math/mathUtil.h"


#define REMAINS_CONSTANT(v) REMAINS_CONSTANT_TOLERANT(v, 0.00005)

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
