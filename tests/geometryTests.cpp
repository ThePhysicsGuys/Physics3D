#include "testsMain.h"

#include "../engine/math/mat3.h"
#include "../engine/math/vec3.h"
#include "../engine/math/mathUtil.h"
#include "../engine/math/utils.h"

#include "../engine/geometry/shape.h"
#include "../engine/geometry/boundingBox.h"

#include "../application/objectLibrary.h"

#include "testValues.h"

#define ASSERT(condition) ASSERT_TOLERANT(condition, 0.00001)

template<typename N, typename Tol>
bool isDiagonalTolerant(Mat3Template<N> m, Tol tolerance) {
	return tolerantEquals(m.m01, 0, tolerance) && tolerantEquals(m.m10, 0, tolerance) &&
		tolerantEquals(m.m02, 0, tolerance) && tolerantEquals(m.m20, 0, tolerance) &&
		tolerantEquals(m.m21, 0, tolerance) && tolerantEquals(m.m12, 0, tolerance);
}
template<typename N, typename Tol>
bool isDiagonalTolerant(SymmetricMat3Template<N> m, Tol tolerance) {
	return tolerantEquals(m.m01, 0, tolerance) && 
		tolerantEquals(m.m02, 0, tolerance) && 
		tolerantEquals(m.m12, 0, tolerance);
}

#define ASSERT_DIAGONAL_TOLERANT(matrix, tolerance) if(!isDiagonalTolerant(matrix, tolerance)) throw AssertionError(__LINE__, errMsg(matrix))
#define ASSERT_DIAGONAL(matrix) ASSERT_DIAGONAL_TOLERANT(matrix, 0.00000001)

TEST_CASE(basicShapes) {
	BoundingBox b{ -1, -1, -1, 1, 1, 1 };
	Vec3f vecBuf[8];

	ASSERT_TRUE(b.toShape(vecBuf).isValid());
}

TEST_CASE(shapeVolume) {
	BoundingBox b{-1, -1, -1, 1, 1, 1};
	Vec3f vecBuf[8];
	Vec3f vecBuf2[8];

	Shape boxShape = b.toShape(vecBuf);

	CFramef transform(Vec3f(0.3f, 0.7f, -3.5f), fromEulerAngles(0.7f, 0.2f, 0.3f));

	Shape transformedShape = boxShape.localToGlobal(transform, vecBuf2);

	logf("Box Volume: %.20f", boxShape.getVolume());
	logf("Box Volume2: %.20f", transformedShape.getVolume());

	logf("Triangle Volume: %.20f", triangleShape.getVolume());

	logf("Icosahedron Volume: %.20f", icosahedron.getVolume());
}

TEST_CASE(shapeCenterOfMass) {
	BoundingBox b{-1, -1, -1, 1, 1, 1};
	Vec3f vecBuf[8];
	Vec3f vecBuf2[8];

	Shape boxShape = b.toShape(vecBuf);

	CFramef transform(Vec3f(0.3f, 0.7f, -3.5f), fromEulerAngles(0.7f, 0.2f, 0.3f));

	Shape transformedShape = boxShape.localToGlobal(transform, vecBuf2);
	
	logf("Center Of Mass Box: %s", str(boxShape.getCenterOfMass()).c_str());
	logf("Center Of Mass Transformed Box: %s", str(transformedShape.getCenterOfMass()).c_str());
}

TEST_CASE(shapeInertiaMatrix) {
	BoundingBox b{-1, -1, -1, 1, 1, 1};
	Vec3f vecBuf[8];
	Vec3f vecBuf2[8];
	Vec3f houseVecBuf[10];
	Vec3f houseVecBuf2[10];


	Shape boxShape = b.toShape(vecBuf);

	CFramef transform(Vec3f(0,0,0), fromEulerAngles(0.7f, 0.2f, 0.3f));
	Shape transformedShape = boxShape.localToGlobal(transform, vecBuf2);

	logf("Inertia of Box: %s", str(boxShape.getInertia()).c_str());

	logf("Inertia of transformed Box: %s", str(transformedShape.getInertia()).c_str());

	Shape h = house;
	Shape newHouse = house.translated(-Vec3f(house.getCenterOfMass()), houseVecBuf);
	Shape rotatedHouse = newHouse.rotated(fromEulerAngles(0.0, 0.3, 0.0), houseVecBuf2);
	logf("Inertia of House: %s", str(newHouse.getInertia()).c_str());
	logf("Inertia of Rotated House: %s", str(rotatedHouse.getInertia()).c_str());
}

TEST_CASE(shapeInertiaRotationInvariance) {
	Vec3f buf1[10]; Vec3f buf2[10]; Shape testShape = house.translated(-Vec3f(house.getCenterOfMass()), buf1);

	Vec3 testMoment = Vec3(0.3, -3.2, 4.8);
	Vec3 momentResult = ~testShape.getInertia() * testMoment;

	logStream << "reference inertia: " << testShape.getInertia() << "\n";

	for(RotMat3 testRotation : rotMatrices) {
		

		Shape rotatedShape = testShape.rotated(testRotation, buf2);

		Vec3 rotatedTestMoment = testRotation * testMoment;
		
		SymmetricMat3 inertia = rotatedShape.getInertia();

		Vec3 rotatedMomentResult = ~inertia * rotatedTestMoment;


		logStream << "testRotation: " << testRotation << "\ninertia: " << inertia << "\n";
		ASSERT(rotatedMomentResult == testRotation * momentResult);
	}
}

TEST_CASE(shapeInertiaEigenValueInvariance) {
	Vec3f buf1[10]; Vec3f buf2[10]; Shape testShape = house.translated(-Vec3f(house.getCenterOfMass()), buf1);

	EigenValues<double> initialEigenValues = testShape.getInertia().getEigenDecomposition().eigenValues;

	for(RotMat3 testRotation : rotMatrices) {
		logStream << testRotation << '\n';

		Shape rotatedShape = testShape.rotated(testRotation, buf2);

		EigenValues<double> newEigenValues = rotatedShape.getInertia().getEigenDecomposition().eigenValues;

		ASSERT(initialEigenValues == newEigenValues);
	}
}

TEST_CASE(cubeContainsPoint) {
	Vec3f buf[8];
	Shape cube = BoundingBox{0,0,0,1,1,1}.toShape(buf);

	ASSERT_TRUE(cube.containsPoint(Vec3f(0.2, 0.2, 0.2)));
	ASSERT_TRUE(cube.containsPoint(Vec3f(0.2, 0.2, 0.8)));
	ASSERT_TRUE(cube.containsPoint(Vec3f(0.2, 0.9, 0.2)));
	ASSERT_TRUE(cube.containsPoint(Vec3f(0.7, 0.2, 0.2)));

	ASSERT_FALSE(cube.containsPoint(Vec3f(1.2, 0.2, 0.2)));
	ASSERT_FALSE(cube.containsPoint(Vec3f(1.2, 1.2, 0.2)));
	ASSERT_FALSE(cube.containsPoint(Vec3f(1.2, 1.2, 1.2)));
	ASSERT_FALSE(cube.containsPoint(Vec3f(-0.2, -0.2, -0.2)));
}

TEST_CASE(testRayIntersection) {
	ASSERT_TRUE(rayTriangleIntersection(Vec3(-1.0, 0.3, 0.3), Vec3(1.0, 0.0, 0.0), Vec3(), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)).rayIntersectsTriangle());
	ASSERT_FALSE(rayTriangleIntersection(Vec3(-1.0, -0.3, 0.3), Vec3(1.0, 0.0, 0.0), Vec3(), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)).rayIntersectsTriangle());
	ASSERT_FALSE(rayTriangleIntersection(Vec3(-1.0, 0.3, -0.3), Vec3(1.0, 0.0, 0.0), Vec3(), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)).rayIntersectsTriangle());
	ASSERT_FALSE(rayTriangleIntersection(Vec3(-1.0, -0.3, -0.3), Vec3(1.0, 0.0, 0.0), Vec3(), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)).rayIntersectsTriangle());
}

#include "../util/log.h"

TEST_CASE(testIntersection) {
	Vec3f buf[12];
	Vec3f buf2[8];
	Vec3f buf3[12];
	Shape a = BoundingBox{-0.3, -0.4, -0.5, 0.3, 0.4, 0.5}.toShape(buf2);
	Shape b = icosahedron.translated(Vec3f(0.8, 0.9, 0.8), buf);
	Shape c = icosahedron.translated(Vec3f(0.95, 0.0, 0.0), buf3);
	

	Vec3f intersect, exitVec;


	Vec3 startingVec(1, 0, 0);

	ASSERT_TRUE(a.intersects(b, intersect, exitVec, startingVec));
	ASSERT_FALSE(a.intersects(c, intersect, exitVec, startingVec));
	/*Vec3 buf1[8];
	Vec3 buf2[8];

	BoundingBox statBox{-0.33, -0.37, -0.73, 0.33, 0.37, 0.73};
	Shape stationary = statBox.toShape(buf1);

	Vec3 i;

	for(double x = -2.0; x < 2.0; x += 0.1) {
		for(double y = -2.0; y < 2.0; y += 0.1) {
			for(double z = -2.0; z < 2.0; z += 0.1) {
				printf("x=%f, y=%f, z=%f", x, y, z);

				BoundingBox movBox{-0.17 + x, -0.57 + y, -0.71 + z, 0.17 + x, 0.57 + y, 0.71 + z};
				Shape moving = movBox.toShape(buf1);

				ASSERT(statBox.intersects(movBox) == stationary.intersects(moving, i));
			}
		}
	}*/
}

TEST_CASE(badCollissions) {
	Triangle triangles[12] = {
		{1,0,2},{3,2,0}, // BOTTOM
		{1,5,0},{4,0,5}, // FRONT
		{1,2,5},{6,5,2}, // RIGHT
		{6,2,7},{3,7,2}, // BACK
		{3,0,7},{4,7,0}, // LEFT
		{4,5,7},{6,7,5}, // TOP
	};

	Shape dominoI(badVerticesI, triangles, 8, 12);
	Shape dominoJ(badVerticesJ, triangles, 8, 12);

	Vec3f intersection;
	Vec3f exitVec;
	dominoI.intersects(dominoJ, intersection, exitVec, Vec3(1, 0, 0));
}

TEST_CASE(testGetFurthestPointInDirection) {
	for (Vec3f vertex : icosahedron.iterVertices()) {
		ASSERT(icosahedron.furthestInDirection(vertex) == vertex);
	}
}
