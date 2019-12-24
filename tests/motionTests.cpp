#include "testsMain.h"

#include "compare.h"
#include "../physics/misc/toString.h"
#include "estimateMotion.h"

#include "../physics/motion.h"

#define REMAINS_CONSTANT(v) REMAINS_CONSTANT_TOLERANT(v, 0.05)
#define ASSERT(v) ASSERT_TOLERANT(v, 0.05)

#define DELTA_T 0.001



TEST_CASE(testBasicMotion) {
	Vec3 vel(1.3, 0.7, 0.9);
	Vec3 angularVel(1.7, 0.9, 1.2);
	Vec3 point(1.2, -0.5, 0.7);

	Motion m(vel, angularVel);

	ASSERT(m.getVelocityOfPoint(point) == getVelocityBySimulation(m, point, DELTA_T));
}

TEST_CASE(testAdvancedMotion) {
	Vec3 vel(1.3, 0.7, 0.9);
	Vec3 angularVel(1.7, 0.9, 1.2);
	Vec3 accel(7.1, 0.9, -3.8);
	Vec3 angularAccel(1.8, 0.67, -4.1);
	Vec3 point(1.2, -0.5, 0.7);

	Motion m(vel, angularVel, accel, angularAccel);

	ASSERT(m.getMotionOfPoint(point) == getMotionBySimulation(m, point, DELTA_T));
}
