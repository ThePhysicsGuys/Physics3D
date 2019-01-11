#include "testsMain.h"

#include "../engine/math/mat3.h"
#include "../engine/math/vec3.h"
#include "../engine/math/mathUtil.h"

#include "../engine/geometry/shape.h"
#include "../engine/geometry/boundingBox.h"

#include "../application/objectLibrary.h"

#define ASSERT(condition) ASSERT_TOLERANT(condition, 0.00000001)

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

	logf("Box Volume: %.20f", boxShape.getVolume());
	logf("Box Volume2: %.20f", transformedShape.getVolume());

	logf("Triangle Volume: %.20f", triangleShape.getVolume());

	logf("Icosahedron Volume: %.20f", icosahedron.getVolume());
}

TEST_CASE(shapeCenterOfMass) {
	BoundingBox b{-1, -1, -1, 1, 1, 1};
	Vec3 vecBuf[8];
	Vec3 vecBuf2[8];

	Shape boxShape = b.toShape(vecBuf);

	CFrame transform = CFrame(Vec3(0.3, 0.7, -3.5), fromEulerAngles(0.7, 0.2, 0.3));

	Shape transformedShape = boxShape.localToGlobal(transform, vecBuf2);
	
	logf("Center Of Mass Box: %s", str(boxShape.getCenterOfMass()).c_str());
	logf("Center Of Mass Transformed Box: %s", str(transformedShape.getCenterOfMass()).c_str());
}

TEST_CASE(shapeInertiaMatrix) {
	BoundingBox b{-1, -1, -1, 1, 1, 1};
	Vec3 vecBuf[8];
	Vec3 vecBuf2[8];
	Vec3 houseVecBuf[10];
	Vec3 houseVecBuf2[10];


	Shape boxShape = b.toShape(vecBuf);

	CFrame transform = CFrame(Vec3(0,0,0), fromEulerAngles(0.7, 0.2, 0.3));
	Shape transformedShape = boxShape.localToGlobal(transform, vecBuf2);

	logf("Inertia of Box: %s", str(boxShape.getInertia()).c_str());

	logf("Inertia of transformed Box: %s", str(transformedShape.getInertia()).c_str());

	Shape h = house;
	Shape newHouse = house.translated(-house.getCenterOfMass(), houseVecBuf);
	Shape rotatedHouse = newHouse.rotated(fromEulerAngles(0.0, 0.3, 0.0), houseVecBuf2);
	logf("Inertia of House: %s", str(newHouse.getInertia()).c_str());
	logf("Inertia of Rotated House: %s", str(rotatedHouse.getInertia()).c_str());
}

TEST_CASE(cubeContainsPoint) {
	Vec3 buf[8];
	Shape cube = BoundingBox{0,0,0,1,1,1}.toShape(buf);

	ASSERT_TRUE(cube.containsPoint(Vec3(0.2, 0.2, 0.2)));
	ASSERT_TRUE(cube.containsPoint(Vec3(0.2, 0.2, 0.8)));
	ASSERT_TRUE(cube.containsPoint(Vec3(0.2, 0.9, 0.2)));
	ASSERT_TRUE(cube.containsPoint(Vec3(0.7, 0.2, 0.2)));

	ASSERT_FALSE(cube.containsPoint(Vec3(1.2, 0.2, 0.2)));
	ASSERT_FALSE(cube.containsPoint(Vec3(1.2, 1.2, 0.2)));
	ASSERT_FALSE(cube.containsPoint(Vec3(1.2, 1.2, 1.2)));
	ASSERT_FALSE(cube.containsPoint(Vec3(-0.2, -0.2, -0.2)));
}
