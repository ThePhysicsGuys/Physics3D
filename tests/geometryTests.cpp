#include "testsMain.h"
#include "testUtils.h"

#include "../engine/geometry/boundingBox.h"

TEST_CASE(basicShapes) {
	BoundingBox b{ -1, -1, -1, 1, 1, 1 };
	Vec3 vecBuf[8];

	ASSERT(b.toShape(vecBuf).isValid());
}