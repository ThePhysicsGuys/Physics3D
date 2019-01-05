#include "testsMain.h"

#include "../engine/math/mat3.h"
#include "../engine/math/mathUtil.h"
#include "../util/log.h"

Mat3 IDENTITY = Mat3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

#define ASSERT(condition) ASSERT_TOLERANT(condition, 0.00000001)

TEST_CASE(matrixInv) {
	Mat3 m(100.0, 0.0, 0.0, 0.0, 50.0, 0.0, 0.0, 0.0, 1.0);

	logf("m=%s\n~m=%s\nm.inverse()=%s\nm.det()=%f", str(m).c_str(), str(~m).c_str(), str(m.inverse()).c_str(), m.det());

	ASSERT(m.det() == 5000.0);

	ASSERT(m * ~m == IDENTITY);
	ASSERT(~m * m == IDENTITY);
}

TEST_CASE(testFromRotationVec) {
	double deltaT = 0.000001;
	Vec3 rotationVec(0.0, 0.0, 1.0);
	Vec3 pointToRotate = Vec3(1.0, 0.0, 0.0);

	Vec3 newPoint = pointToRotate + pointToRotate % (rotationVec*deltaT);

	Mat3 fromRotV = fromRotationVec(rotationVec * deltaT);

	Vec3 newPoint2 = fromRotV * pointToRotate;

	logf("rotVec: %s, rotMat: %s", str(rotationVec).c_str(), str(fromRotV).c_str());
	logf("\nnewPointRotVec: %s\nnewPointRotMat: %s", str(newPoint).c_str(), str(newPoint2).c_str());
}
