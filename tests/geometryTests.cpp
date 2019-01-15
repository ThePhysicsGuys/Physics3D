#include "testsMain.h"

#include "../engine/math/mat3.h"
#include "../engine/math/vec3.h"
#include "../engine/math/mathUtil.h"

#include "../engine/geometry/shape.h"
#include "../engine/geometry/boundingBox.h"

#include "../application/objectLibrary.h"

#include "testValues.h"

#define ASSERT(condition) ASSERT_TOLERANT(condition, 0.00000001)

template<typename N, typename Tol>
bool isDiagonalTolerant(Mat3Template<N> m, Tol tolerance) {
	return tolerantEquals(m.m01, 0, tolerance) && tolerantEquals(m.m10, 0, tolerance) &&
		tolerantEquals(m.m02, 0, tolerance) && tolerantEquals(m.m20, 0, tolerance) &&
		tolerantEquals(m.m21, 0, tolerance) && tolerantEquals(m.m12, 0, tolerance);
}

#define ASSERT_DIAGONAL_TOLERANT(matrix, tolerance) if(!isDiagonalTolerant(matrix, tolerance)) throw AssertionError(__LINE__, errMsg(matrix))
#define ASSERT_DIAGONAL(matrix) ASSERT_DIAGONAL_TOLERANT(matrix, 0.00000001)

TEST_CASE(basicShapes) {
	BoundingBox b{ -1, -1, -1, 1, 1, 1 };
	Vec3 vecBuf[8];

	ASSERT_TRUE(b.toShape(vecBuf).isValid());
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

TEST_CASE(shapeInertiaRotationInvariance) {
	Vec3 buf1[10]; Vec3 buf2[10]; Shape testShape = house.translated(-house.getCenterOfMass(), buf1);

	Vec3 testMoment = Vec3(0.3, -3.2, 4.8);
	Vec3 momentResult = ~testShape.getInertia() * testMoment;

	logStream << "reference inertia: " << testShape.getInertia() << "\n";

	for(RotMat3 testRotation : rotMatrices) {
		

		Shape rotatedShape = testShape.rotated(testRotation, buf2);

		Vec3 rotatedTestMoment = testRotation * testMoment;
		
		Mat3 inertia = rotatedShape.getInertia();

		Vec3 rotatedMomentResult = ~inertia * rotatedTestMoment;


		logStream << "testRotation: " << testRotation << "\ninertia: " << inertia << "\n";
		ASSERT(rotatedMomentResult == testRotation * momentResult);
	}
}

TEST_CASE(shapeInertiaEigenValueInvariance) {
	Vec3 buf1[10]; Vec3 buf2[10]; Shape testShape = house.translated(-house.getCenterOfMass(), buf1);

	EigenValues<double> initialEigenValues = testShape.getInertia().getEigenDecomposition().eigenValues;

	for(RotMat3 testRotation : rotMatrices) {
		logStream << testRotation << '\n';

		Shape rotatedShape = testShape.rotated(testRotation, buf2);

		EigenValues<double> newEigenValues = rotatedShape.getInertia().getEigenDecomposition().eigenValues;

		ASSERT(initialEigenValues == newEigenValues);
	}
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

TEST_CASE(shapeNormalization) {
	Vec3 buf1[10]; Shape transformedHouse = house.globalToLocal(CFrame(Vec3(0.3, 0.7, -2.8), fromEulerAngles(0.5, -0.1, 0.9)), buf1);

	double beginVolume = transformedHouse.getVolume();

	transformedHouse.normalize();

	ASSERT(transformedHouse.getVolume() == beginVolume);
	ASSERT(transformedHouse.getCenterOfMass() == Vec3());
	ASSERT_DIAGONAL(transformedHouse.getInertia());
}
