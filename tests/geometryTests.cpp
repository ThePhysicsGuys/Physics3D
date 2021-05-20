#include "testsMain.h"

#include "compare.h"
#include "../physics/misc/toString.h"

#include "../physics/math/linalg/vec.h"
#include "../physics/math/linalg/mat.h"
#include "../physics/math/linalg/trigonometry.h"
#include "../physics/math/linalg/eigen.h"
#include "../physics/math/utils.h"
#include "../physics/math/boundingBox.h"

#include "../physics/geometry/shape.h"

#include "../physics/misc/shapeLibrary.h"

#include "testValues.h"
#include "generators.h"

#include "../util/cpuid.h"

using namespace P3D;
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
	for(size_t row = 0; row < Size - 1; row++) {
		for(size_t col = row + 1; col < Size; col++) {
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

	CFramef transform(Vec3f(0.3f, 0.7f, -3.5f), Rotationf::fromEulerAngles(0.7f, 0.2f, 0.3f));

	Polyhedron transformedShape = boxShape.localToGlobal(transform);

	ASSERT(boxShape.getVolume() == transformedShape.getVolume());
	ASSERT(boxShape.getVolume() == 8.0);
}

TEST_CASE(shapeCenterOfMass) {
	Polyhedron boxShape = Library::createBox(2.0, 2.0, 2.0);

	CFramef transform(Vec3f(0.3f, 0.7f, -3.5f), Rotationf::fromEulerAngles(0.7f, 0.2f, 0.3f));

	Polyhedron transformedShape = boxShape.localToGlobal(transform);

	ASSERT(transform.localToGlobal(boxShape.getCenterOfMass()) == transformedShape.getCenterOfMass());
}

/*TEST_CASE(shapeInertiaMatrix) {
	Polyhedron boxShape = Library::createBox(2.0, 2.0, 2.0);

	CFramef transform(Vec3f(0,0,0), rotationMatrixfromEulerAngles(0.7f, 0.2f, 0.3f));
	Polyhedron transformedShape = boxShape.localToGlobal(transform);

	logf("Inertia of boxShape: %s", str(boxShape.getInertiaAroundCenterOfMass()).c_str());

	logf("Inertia of transformed boxShape: %s", str(transformedShape.getInertiaAroundCenterOfMass()).c_str());

	Polyhedron h = Library::house;
	Polyhedron newHouse = Library::house.translated(-Vec3f(Library::house.getCenterOfMass()));
	Polyhedron rotatedHouse = newHouse.rotated(rotationMatrixfromEulerAngles(0.0, 0.3, 0.0));
	logf("Inertia of House: %s", str(newHouse.getInertiaAroundCenterOfMass()).c_str());
	logf("Inertia of Rotated House: %s", str(rotatedHouse.getInertiaAroundCenterOfMass()).c_str());
}*/

TEST_CASE(shapeInertiaRotationInvariance) {
	Polyhedron testShape = Library::house.translated(-Vec3f(Library::house.getCenterOfMass()));

	Vec3 testMoment = Vec3(0.7, -3.2, 4.8);
	Vec3 momentResult = ~testShape.getInertiaAroundCenterOfMass() * testMoment;

	logStream << "Center Of Mass: " << testShape.getCenterOfMass() << '\n';

	logStream << "reference inertia: " << testShape.getInertiaAroundCenterOfMass() << "\n";

	logStream << "Inertial eigenValues: " << getEigenDecomposition(testShape.getInertiaAroundCenterOfMass()).eigenValues.asDiagonalMatrix() << "\n";

	for(const Rotation& testRotation : rotations) {


		Polyhedron rotatedShape = testShape.rotated(static_cast<Rotationf>(testRotation));

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

	for(Rotation testRotation : rotations) {
		logStream << testRotation << '\n';

		Polyhedron rotatedShape = testShape.rotated(static_cast<Rotationf>(testRotation));

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
	for(Vec3f vertex : Library::icosahedron.iterVertices()) {
		ASSERT(Library::icosahedron.furthestInDirection(vertex) == vertex);
	}
}

TEST_CASE(testTriangleMeshOptimizedGetBounds) {
	for(int iter = 0; iter < 1000; iter++) {
		TriangleMesh mesh = generateTriangleMesh();
		logStream << "NewPoly: " << mesh.vertexCount << " vertices, " << mesh.triangleCount << " triangles\n";
		for(int i = 0; i < mesh.vertexCount; i++) {
			logStream << mesh.getVertex(i) << "\n";
		}
		BoundingBox reference = mesh.getBoundsFallback();

		if(Util::CPUIDCheck::hasTechnology(Util::CPUIDCheck::SSE | Util::CPUIDCheck::SSE2)) {
			ASSERT(reference == mesh.getBoundsSSE());
		}

		if(Util::CPUIDCheck::hasTechnology(Util::CPUIDCheck::AVX | Util::CPUIDCheck::AVX2 | Util::CPUIDCheck::FMA)) {
			ASSERT(reference == mesh.getBoundsAVX());
		}
	}
}

TEST_CASE(testTriangleMeshOptimizedGetBoundsRotated) {
	for(int iter = 0; iter < 1000; iter++) {
		TriangleMesh mesh = generateTriangleMesh();
		logStream << "NewPoly: " << mesh.vertexCount << " vertices, " << mesh.triangleCount << " triangles\n";
		for(int i = 0; i < mesh.vertexCount; i++) {
			logStream << mesh.getVertex(i) << "\n";
		}
		Mat3f rot = generateMatrix<float, 3, 3>();
		BoundingBox reference = mesh.getBoundsFallback(rot);

		if(Util::CPUIDCheck::hasTechnology(Util::CPUIDCheck::SSE | Util::CPUIDCheck::SSE2)) {
			ASSERT(reference == mesh.getBoundsSSE(rot));
		}

		if(Util::CPUIDCheck::hasTechnology(Util::CPUIDCheck::AVX | Util::CPUIDCheck::AVX2 | Util::CPUIDCheck::FMA)) {
			ASSERT(reference == mesh.getBoundsAVX(rot));
		}
	}
}

TEST_CASE(testTriangleMeshOptimizedFurthestIndexInDirection) {
	for(int iter = 0; iter < 1000; iter++) {
		TriangleMesh mesh = generateTriangleMesh();
		logStream << "NewPoly: " << mesh.vertexCount << " vertices, " << mesh.triangleCount << " triangles\n";
		for(int i = 0; i < mesh.vertexCount; i++) {
			logStream << mesh.getVertex(i) << "\n";
		}
		Vec3 dir = generateVec3();
		int reference = mesh.furthestIndexInDirectionFallback(dir);
		logStream << "reference: " << reference << "\n";

		if(Util::CPUIDCheck::hasTechnology(Util::CPUIDCheck::SSE | Util::CPUIDCheck::SSE2)) {
			int sseVertex = mesh.furthestIndexInDirectionSSE(dir);
			logStream << "sseVertex: " << sseVertex << "\n";
			ASSERT(mesh.getVertex(reference) * dir == mesh.getVertex(sseVertex) * dir);
		}

		if(Util::CPUIDCheck::hasTechnology(Util::CPUIDCheck::SSE | Util::CPUIDCheck::SSE2 | Util::CPUIDCheck::SSE4_1)) {
			int sse4Vertex = mesh.furthestIndexInDirectionSSE4(dir);
			logStream << "sse4Vertex: " << sse4Vertex << "\n";
			ASSERT(mesh.getVertex(reference) * dir == mesh.getVertex(sse4Vertex) * dir);
		}

		if(Util::CPUIDCheck::hasTechnology(Util::CPUIDCheck::AVX | Util::CPUIDCheck::AVX2 | Util::CPUIDCheck::FMA)) {
			int avxVertex = mesh.furthestIndexInDirectionAVX(dir);
			logStream << "avxVertex: " << avxVertex << "\n";
			ASSERT(mesh.getVertex(reference) * dir == mesh.getVertex(avxVertex) * dir);
		}
	}
}
TEST_CASE(testTriangleMeshOptimizedFurthestInDirection) {
	for(int iter = 0; iter < 1000; iter++) {
		TriangleMesh mesh = generateTriangleMesh();
		logStream << "NewPoly: " << mesh.vertexCount << " vertices, " << mesh.triangleCount << " triangles\n";
		for(int i = 0; i < mesh.vertexCount; i++) {
			logStream << mesh.getVertex(i) << "\n";
		}
		Vec3 dir = generateVec3();
		Vec3 reference = mesh.furthestInDirectionFallback(dir);
		logStream << "reference: " << reference << "\n";

		if(Util::CPUIDCheck::hasTechnology(Util::CPUIDCheck::SSE | Util::CPUIDCheck::SSE2)) {
			Vec3f sseVertex = mesh.furthestInDirectionSSE(dir);
			logStream << "sseVertex: " << sseVertex << "\n";
			ASSERT(reference * dir == sseVertex * dir); // dot with dir as we don't really care for the exact vertex in a tie
		}

		if(Util::CPUIDCheck::hasTechnology(Util::CPUIDCheck::SSE | Util::CPUIDCheck::SSE2 | Util::CPUIDCheck::SSE4_1)) {
			Vec3f sse4Vertex = mesh.furthestInDirectionSSE4(dir);
			logStream << "sse4Vertex: " << sse4Vertex << "\n";
			ASSERT(reference * dir == sse4Vertex * dir); // dot with dir as we don't really care for the exact vertex in a tie
		}

		if(Util::CPUIDCheck::hasTechnology(Util::CPUIDCheck::AVX | Util::CPUIDCheck::AVX2 | Util::CPUIDCheck::FMA)) {
			Vec3f avxVertex = mesh.furthestInDirectionAVX(dir);
			logStream << "avxVertex: " << avxVertex << "\n";
			ASSERT(reference * dir == avxVertex * dir); // dot with dir as we don't really care for the exact vertex in a tie
		}
	}
}
