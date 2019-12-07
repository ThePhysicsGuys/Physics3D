#include "testsMain.h"

#include "../physics/math/linalg/vec.h"
#include "../physics/math/linalg/mat.h"
#include "../physics/math/linalg/trigonometry.h"
#include "../physics/math/linalg/misc.h"
#include "../physics/math/linalg/largeMatrix.h"
#include "../physics/math/linalg/eigen.h"
#include "../physics/math/mathUtil.h"

#define ASSERT(condition) ASSERT_TOLERANT(condition, 0.00000001)

TEST_CASE(subMatrixOperations) {
	Matrix<int, 3, 5> mat{
		5,7,9,
		6,9,2,
		3,9,5,
		3,8,4,
		3,8,1
	};

	ASSERT((mat.getSubMatrix<2, 3>(1, 0)) == (Matrix<int, 2, 3>{
		6, 9,
		3, 9,
		3, 8
	}));

	ASSERT((mat.withoutRow(2)) == (Matrix<int, 3, 4>{
		5, 7, 9,
		6, 9, 2,
		3, 8, 4,
		3, 8, 1
	}));

	ASSERT((mat.withoutCol(1)) == (Matrix<int, 2, 5>{
		5, 9,
		6, 2,
		3, 5,
		3, 4,
		3, 1
	}));
}

TEST_CASE(determinant) {
	Matrix<double, 3, 3> m3{
		1, 7, 21,
		7, 6, 9, 
		1, 3, 8
	};

	logf("m=%s\nm.det()=%f", str(m3).c_str(), det(m3));

	ASSERT(det(m3) == 7);


	Mat4 m4{
		1, 7, 21, 6, 
		7, 6, 9, 8, 
		1, 3, 8, 4, 
		2, 9, 7, 3
	};

	logf("m=%s\nm.det()=%f", str(m4).c_str(), det(m4));
	ASSERT(det(m4) == -983);
}


TEST_CASE(matrixInverse2) {
	Mat2 m{ 4,7,9,3};

	logf("m=%s\n~m=%s\nm.inverse()=%s\nm.det()=%f", str(m).c_str(), str(~m).c_str(), str(~m).c_str(), det(m));

	ASSERT(det(m) * det(~m) == 1.0);
	ASSERT(m * ~m == Mat2::IDENTITY());
	ASSERT(~m * m == Mat2::IDENTITY());
}

TEST_CASE(matrixInverse3) {
	Mat3 m{ 1,3,4,7,6,9,5,3,2 };
	SymmetricMat3 s{1, 4, 7, 6, 9, 8};
	DiagonalMat3 d{ 7, 5, 3 };

	logf("m=%s\n~m=%s\nm.det()=%f", str(m).c_str(), str(~m).c_str(), det(m));

	ASSERT(m * ~m == Mat3::IDENTITY());
	ASSERT(~m * m == Mat3::IDENTITY());
	ASSERT(s * ~s == SymmetricMat3::IDENTITY());
	ASSERT(~s * s == SymmetricMat3::IDENTITY());
	ASSERT(d * ~d == DiagonalMat3::IDENTITY());
	ASSERT(~d * d == DiagonalMat3::IDENTITY());
}

TEST_CASE(matrixInverse4) {
	Mat4 m{ 
		1, 3, 4, 7, 
		7, 9, 3, 5, 
		4, 9, 1, 2, 
		6, 7, 6, 9};

	logf("m=%s\n~m=%s\nm.inverse()=%s\nm.det()=%f", str(m).c_str(), str(~m).c_str(), str(~m).c_str(), det(m));

	ASSERT(det(m) * det(~m) == 1.0);

	ASSERT(m * ~m == Mat4::IDENTITY());
	ASSERT(~m * m == Mat4::IDENTITY());
}

TEST_CASE(cframeInverse) {
	CFrame A(Vec3(1.0, 0.8, 0.9), fromEulerAngles(0.3, 0.7, 0.9));
	CFrame B(Vec3(8.2, -0.8, 3.4), fromEulerAngles(0.4, 0.4, 0.3));

	ASSERT(A.localToGlobal(A.globalToLocal(B)) == B);
	ASSERT(A.globalToLocal(A.localToGlobal(B)) == B);
	ASSERT(A.globalToLocal(A) == CFrame());
	ASSERT(A.localToGlobal(~A) == CFrame());
	ASSERT((~A).localToGlobal(A) == CFrame());

	ASSERT(A.localToRelative(B) + A.position == A.localToGlobal(B));
}

TEST_CASE(testFromRotationVec) {
	double deltaT = 0.0001;

	Vec3 rotations[]{Vec3(1,0,0), Vec3(0,1,0), Vec3(0,0,1), Vec3(-5.0, 3.0, 9.0), Vec3(3.8, -0.5, 0.4)};
	for(Vec3 rot : rotations) {
		Vec3 rotationVec = rot * deltaT;
		Vec3 pointToRotate = Vec3(1.0, 0.0, 0.0);

		Mat3 rotMat = fromRotationVec(rotationVec);

		Vec3 rotateByRotVec = pointToRotate;
		Vec3 rotateByRotMat = pointToRotate;

		for(int i = 0; i < 500; i++) {
			rotateByRotVec = rotateByRotVec + rotationVec % rotateByRotVec;
			rotateByRotMat = rotMat * rotateByRotMat;
		}

		logStream << "rotVec: " << rotationVec << ", rotMat: " << rotMat << '\n';
		logStream << "newPointRotVec: " << rotateByRotVec << "\nnewPointRotMat: " << rotateByRotMat << '\n';

		ASSERT_TOLERANT(rotateByRotVec == rotateByRotMat, 0.01);
	}
}

TEST_CASE(matrixAssociativity) {

	Mat3 A3{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	Mat3 B3{ 11, 13, 17, 19, 23, 29, 31, 37, 41 };
	Mat3 C3{ 0.1, 0.2, 0.7, 0.9, -0.3, -0.5, 0.9, 0.1, -0.3 };

	ASSERT((A3*B3)*C3 == A3*(B3*C3));

	Vec3 v3(17, -0.7, 9.4);

	ASSERT((A3*B3)*v3 == (A3*(B3*v3)));


	Mat4 A4{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	Mat4 B4{ 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 57, 61, 67, 71 };
	Mat4 C4{ 0.1, 0.2, 0.7, 0.9, -0.3, -0.5, 0.9, 0.1, -0.3, -0.8, 1.6, 0.4, 0.7, 0.3, 0.1, 0.1 };

	ASSERT((A4*B4)*C4 == A4*(B4*C4));

	Vec4 v4(17, -0.7, 9.4, 0.888);

	ASSERT((A4*B4)*v4 == (A4*(B4*v4)));
}

TEST_CASE(cframeAssociativity) {

	CFrame A(Vec3(0.7, 1.02, 0.9), fromEulerAngles(0.7, 0.9, 0.3));
	CFrame B(Vec3(-0.2, 10.02, 0.3), fromEulerAngles(0.2, 0.2, 0.1));
	CFrame C(Vec3(-0.2343657, 17.02, -9.3), fromEulerAngles(0.9, 0.4, 0.9));

	Vec3 v(17, -0.7, 9.4);

	ASSERT(A.localToGlobal(B).localToGlobal(C) == A.localToGlobal(B.localToGlobal(C)));
	ASSERT(A.localToGlobal(B).localToGlobal(v) == A.localToGlobal(B).localToGlobal(v));
}

TEST_CASE(cframeMat4Equivalence) {
	CFrame A(Vec3(0.7, 1.02, 0.9), fromEulerAngles(0.7, 0.9, 0.3));
	Mat4 A4 = CFrameToMat4(A);

	Vec3 v(17, -0.7, 9.4);

	ASSERT(Mat4ToCFrame(CFrameToMat4(A)) == A);
	ASSERT(A.localToGlobal(v) == Vec3(CFrameToMat4(A) * Vec4(v, 1.0)));
	ASSERT(A.globalToLocal(v) == Vec3(~CFrameToMat4(A) * Vec4(v, 1.0)));
	ASSERT(Vec3(A4 * Vec4(v, 1.0)) == Mat4ToCFrame(A4).localToGlobal(v));
	ASSERT(Vec3(~A4 * Vec4(v, 1.0)) == Mat4ToCFrame(A4).globalToLocal(v));
}

TEST_CASE(fromEuler) {
	ASSERT(rotZ(0.5)*rotX(0.3)*rotY(0.7) == fromEulerAngles(0.3, 0.7, 0.5));
}

TEST_CASE(crossProduct) {
	Vec3 x(1.0, 0.0, 0.0);
	Vec3 y(0.0, 1.0, 0.0);
	Vec3 z(0.0, 0.0, 1.0);
	Vec3 v(1.3, 9.2, -3.7);
	Vec3 u(-7.3, 1.8, 0.5);

	ASSERT(v % u == -(u % v));
	ASSERT((2.0 * v) % (3.0 * u) == 6.0 * (v % u));

	ASSERT(x % y == z);
	ASSERT(y % x == -z);
	ASSERT(y % z == x);
	ASSERT(z % y == -x);
	ASSERT(z % x == y);
	ASSERT(x % z == -y);
}

TEST_CASE(matrixTypes) {
	Mat3 a{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	SymmetricMat3 sym{ 2, 2, 2, 7, 8, 9 };
	DiagonalMat3 dia{ 1,2,5 };

	Mat3 asym = a*sym;
	Mat3 syma = sym*a;

	SymmetricMat3 symdia = sym*dia;
	SymmetricMat3 diasym = dia*sym;

	SymmetricMat3 symsym = sym*sym;

	DiagonalMat3 diadia = dia*dia;
}

#include <iostream>
#include <algorithm>

TEST_CASE(eigenDecomposition) {
	for(double x = -1.25; x < 1.5; x += 0.1) {
		for(double y = -1.35; y < 1.5; y += 0.1) {
			for(double z = -1.55; z < 1.5; z += 0.1) {
				RotMat3 orthoPart = fromEulerAngles(0.21, 0.31, 0.41);
				DiagonalMat3 eigenMat{x,y,z};

				SymmetricMat3 testMat = transformBasis(SymmetricMat3(eigenMat), orthoPart);

				EigenSet<double, 3> v = getEigenDecomposition(testMat);

				EigenValues<double, 3> realValues{x,y,z};

				ASSERT(realValues == v.eigenValues);
				ASSERT(transformBasis(v.eigenValues.asDiagonalMatrix(), v.eigenVectors) == testMat);
			}
		}
	}
}

TEST_CASE(largeMatrixVectorProduct) {
	LargeMatrix<double> mat(5, 5);
	LargeVector<double> vec(5);

	for (int i = 0; i < 5; i++) {
		vec[i] = i + 2;
	}

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			mat.get(i, j) = 0;
		}
		mat.get(i, 0) = i * 2;
		mat.get(0, i) = i * 2;
	}

	LargeVector<double> newVector = mat * vec;
}

TEST_CASE(largeMatrixVectorSolve) {
	LargeMatrix<double> mat(5, 5);
	LargeVector<double> vec(5);

	for (int i = 0; i < 5; i++) {
		vec[i] = fRand(-1.0, 1.0);
	}

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			mat.get(i,j) = fRand(-1.0, 1.0);
		}
	}

	mat.get(0,0) = 0;

	LargeVector<double> newVector = mat * vec;

	LargeVector<double> solutionVector = newVector;

	destructiveSolve(mat, solutionVector);

	ASSERT(solutionVector == vec);
}
