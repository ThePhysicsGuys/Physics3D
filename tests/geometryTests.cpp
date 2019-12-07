#include "testsMain.h"

#include "../physics/math/linalg/vec.h"
#include "../physics/math/linalg/mat.h"
#include "../physics/math/linalg/trigonometry.h"
#include "../physics/math/linalg/eigen.h"
#include "../physics/math/mathUtil.h"
#include "../physics/math/utils.h"

#include "../physics/geometry/shape.h"
#include "../physics/geometry/boundingBox.h"

#include "../physics/misc/shapeLibrary.h"

#include "testValues.h"

#define ASSERT(condition) ASSERT_TOLERANT(condition, 0.00001)

template<typename T, typename Tol, size_t Size>
bool isDiagonalTolerant(const Matrix<T, Size, Size>& m, Tol tolerance) {
	for(size_t row = 0; row < Size; row++) {
		for(size_t col = 0; col < Size; col++) {
			if(row == col) continue;
			if(!tolerantEquals<T>(m[row][col], 0, tolerance)) {
				return false;
			}
		}
	}
	return true;
}
template<typename T, typename Tol, size_t Size>
bool isDiagonalTolerant(const SymmetricMatrix<T, Size>& m, Tol tolerance) {
	for(size_t row = 0; row < Size-1; row++) {
		for(size_t col = row+1; col < Size; col++) {
			if(!tolerantEquals<T>(m[row][col], 0, tolerance)) {
				return false;
			}
		}
	}
	return true;
}

template<typename T, typename Tol, size_t Size>
bool isSymmetricTolerant(const Matrix<T, Size, Size>& m, Tol tolerance) {
	for(size_t row = 0; row < Size - 1; row++) {
		for(size_t col = row + 1; col < Size; col++) {
			if(!tolerantEquals<T>(m[row][col], m[col][row], tolerance)) {
				return false;
			}
		}
	}
	return true;
}

#define ASSERT_DIAGONAL_TOLERANT(matrix, tolerance) if(!isDiagonalTolerant(matrix, tolerance)) throw AssertionError(__LINE__, errMsg(matrix))
#define ASSERT_DIAGONAL(matrix) ASSERT_DIAGONAL_TOLERANT(matrix, 0.00000001)

TEST_CASE(shapeVolume) {
	Polyhedron boxShape = Library::createBox(2, 2, 2);

	CFramef transform(Vec3f(0.3f, 0.7f, -3.5f), fromEulerAngles(0.7f, 0.2f, 0.3f));

	Polyhedron transformedShape = boxShape.localToGlobal(transform);

	ASSERT(boxShape.getVolume() == transformedShape.getVolume());
	ASSERT(boxShape.getVolume() == 8.0);
}

TEST_CASE(shapeCenterOfMass) {
	Polyhedron boxShape = Library::createBox(2.0, 2.0, 2.0);

	CFramef transform(Vec3f(0.3f, 0.7f, -3.5f), fromEulerAngles(0.7f, 0.2f, 0.3f));

	Polyhedron transformedShape = boxShape.localToGlobal(transform);
	
	ASSERT("Center Of Mass Box: %s", transform.localToGlobal(boxShape.getCenterOfMass()) == transformedShape.getCenterOfMass());
}

TEST_CASE(shapeInertiaMatrix) {
	Polyhedron boxShape = Library::createBox(2.0, 2.0, 2.0);

	CFramef transform(Vec3f(0,0,0), fromEulerAngles(0.7f, 0.2f, 0.3f));
	Polyhedron transformedShape = boxShape.localToGlobal(transform);

	logf("Inertia of Box: %s", str(boxShape.getInertiaAroundCenterOfMass()).c_str());

	logf("Inertia of transformed Box: %s", str(transformedShape.getInertiaAroundCenterOfMass()).c_str());

	Polyhedron h = Library::house;
	Polyhedron newHouse = Library::house.translated(-Vec3f(Library::house.getCenterOfMass()));
	Polyhedron rotatedHouse = newHouse.rotated(fromEulerAngles(0.0, 0.3, 0.0));
	logf("Inertia of House: %s", str(newHouse.getInertiaAroundCenterOfMass()).c_str());
	logf("Inertia of Rotated House: %s", str(rotatedHouse.getInertiaAroundCenterOfMass()).c_str());
}

TEST_CASE(shapeInertiaRotationInvariance) {
	Polyhedron testShape = Library::house.translated(-Vec3f(Library::house.getCenterOfMass()));

	Vec3 testMoment = Vec3(0.7, -3.2, 4.8);
	Vec3 momentResult = ~testShape.getInertiaAroundCenterOfMass() * testMoment;

	logStream << "Center Of Mass: " << testShape.getCenterOfMass() << '\n';

	logStream << "reference inertia: " << testShape.getInertiaAroundCenterOfMass() << "\n";

	logStream << "Inertial eigenValues: " << getEigenDecomposition(testShape.getInertiaAroundCenterOfMass()).eigenValues.asDiagonalMatrix() << "\n";

	for(const RotMat3& testRotation : rotMatrices) {
		

		Polyhedron rotatedShape = testShape.rotated(testRotation);

		Vec3 rotatedTestMoment = testRotation * testMoment;
		
		SymmetricMat3 inertia = rotatedShape.getInertiaAroundCenterOfMass();

		Vec3 rotatedMomentResult = ~inertia * rotatedTestMoment;


		logStream << "testRotation: " << testRotation << "\ninertia: " << inertia << "\n";

		logStream << "Inertial eigenValues: " << getEigenDecomposition(inertia).eigenValues.asDiagonalMatrix() << "\n";

		ASSERT(rotatedMomentResult == testRotation * momentResult);
	}
}

TEST_CASE(shapeInertiaEigenValueInvariance) {
	Polyhedron testShape = Library::house.translated(-Vec3f(Library::house.getCenterOfMass()));

	EigenValues<double, 3> initialEigenValues = getEigenDecomposition(testShape.getInertiaAroundCenterOfMass()).eigenValues;

	for(RotMat3 testRotation : rotMatrices) {
		logStream << testRotation << '\n';

		Polyhedron rotatedShape = testShape.rotated(testRotation);

		EigenValues<double, 3> newEigenValues = getEigenDecomposition(rotatedShape.getInertiaAroundCenterOfMass()).eigenValues;

		ASSERT(initialEigenValues == newEigenValues);
	}
}

TEST_CASE(testRayIntersection) {
	ASSERT_TRUE(rayTriangleIntersection(Vec3(-1.0, 0.3, 0.3), Vec3(1.0, 0.0, 0.0), Vec3(), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)).rayIntersectsTriangle());
	ASSERT_FALSE(rayTriangleIntersection(Vec3(-1.0, -0.3, 0.3), Vec3(1.0, 0.0, 0.0), Vec3(), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)).rayIntersectsTriangle());
	ASSERT_FALSE(rayTriangleIntersection(Vec3(-1.0, 0.3, -0.3), Vec3(1.0, 0.0, 0.0), Vec3(), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)).rayIntersectsTriangle());
	ASSERT_FALSE(rayTriangleIntersection(Vec3(-1.0, -0.3, -0.3), Vec3(1.0, 0.0, 0.0), Vec3(), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)).rayIntersectsTriangle());
}

TEST_CASE(testGetFurthestPointInDirection) {
	for (Vec3f vertex : Library::icosahedron.iterVertices()) {
		ASSERT(Library::icosahedron.furthestInDirection(vertex) == vertex);
	}
}
