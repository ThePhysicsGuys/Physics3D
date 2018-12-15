#include "testsMain.h"
#include "testUtils.h"

#include "../engine/math/mat3.h"
#include "../engine/math/vec3.h"
#include "../engine/math/mathUtil.h"

#include "../engine/geometry/shape.h"
#include "../engine/geometry/boundingBox.h"

#include "../util/log.h"

#include "../application/objectLibrary.h"

TEST_CASE(basicShapes) {
	BoundingBox b{ -1, -1, -1, 1, 1, 1 };
	Vec3 vecBuf[8];

	ASSERT(b.toShape(vecBuf).isValid());
}

TEST_CASE(shapeVolume) {
	BoundingBox b{-1, -1, -1, 1, 1, 1};
	Vec3 vecBuf[8];
	Vec3 vecBuf2[8];

	Shape boxShape = b.toShape(vecBuf);

	CFrame transform = CFrame(Vec3(0.3, 0.7, -3.5), fromEulerAngles(0.7, 0.2, 0.3));

	Shape transformedShape = boxShape.localToGlobal(transform, vecBuf2);

	Log::debug("Box Volume: %.20f", boxShape.getVolume());
	Log::debug("Box Volume2: %.20f", transformedShape.getVolume());

	Log::debug("Triangle Volume: %.20f", triangleShape.getVolume());

	Log::debug("Icosahedron Volume: %.20f", icosahedron.getVolume());
}

TEST_CASE(shapeCenterOfMass) {
	BoundingBox b{-1, -1, -1, 1, 1, 1};
	Vec3 vecBuf[8];
	Vec3 vecBuf2[8];

	Shape boxShape = b.toShape(vecBuf);

	CFrame transform = CFrame(Vec3(0.3, 0.7, -3.5), fromEulerAngles(0.7, 0.2, 0.3));

	Shape transformedShape = boxShape.localToGlobal(transform, vecBuf2);
	
	Log::debug("Center Of Mass Box: %s", str(boxShape.getCenterOfMass()).c_str());
	Log::debug("Center Of Mass Transformed Box: %s", str(transformedShape.getCenterOfMass()).c_str());
}

TEST_CASE(cubeContainsPoint) {
	Vec3 buf[8];
	Shape cube = BoundingBox{0,0,0,1,1,1}.toShape(buf);

	ASSERT(cube.containsPoint(Vec3(0.2, 0.2, 0.2)));
	ASSERT(cube.containsPoint(Vec3(0.2, 0.2, 0.8)));
	ASSERT(cube.containsPoint(Vec3(0.2, 0.9, 0.2)));
	ASSERT(cube.containsPoint(Vec3(0.7, 0.2, 0.2)));

	ASSERT(!cube.containsPoint(Vec3(1.2, 0.2, 0.2)));
	ASSERT(!cube.containsPoint(Vec3(1.2, 1.2, 0.2)));
	ASSERT(!cube.containsPoint(Vec3(1.2, 1.2, 1.2)));
	ASSERT(!cube.containsPoint(Vec3(-0.2, -0.2, -0.2)));

}