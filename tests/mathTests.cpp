#include "testsMain.h"

#include "../engine/math/mat3.h"
#include "../engine/math/mat4.h"
#include "../engine/math/mathUtil.h"

Mat3 IDENTITY = Mat3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

#define ASSERT(condition) ASSERT_TOLERANT(condition, 0.00000001)

TEST_CASE(matrixInverse) {
	Mat3 m(100.0, 0.0, 0.0, 0.0, 50.0, 0.0, 0.0, 0.0, 1.0);

	logf("m=%s\n~m=%s\nm.inverse()=%s\nm.det()=%f", str(m).c_str(), str(~m).c_str(), str(m.inverse()).c_str(), m.det());

	ASSERT(m.det() == 5000.0);

	ASSERT(m * ~m == IDENTITY);
	ASSERT(~m * m == IDENTITY);
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

	Mat3 A3(1, 2, 3, 4, 5, 6, 7, 8, 9);
	Mat3 B3(11, 13, 17, 19, 23, 29, 31, 37, 41);
	Mat3 C3(0.1, 0.2, 0.7, 0.9, -0.3, -0.5, 0.9, 0.1, -0.3);

	ASSERT((A3*B3)*C3 == A3*(B3*C3));

	Vec3 v3(17, -0.7, 9.4);

	ASSERT((A3*B3)*v3 == (A3*(B3*v3)));


	Mat4 A4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
	Mat4 B4(11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 57, 61, 67, 71);
	Mat4 C4(0.1, 0.2, 0.7, 0.9, -0.3, -0.5, 0.9, 0.1, -0.3, -0.8, 1.6, 0.4, 0.7, 0.3, 0.1, 0.1);

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
	Mat4 A4 = A.asMat4();

	Vec3 v(17, -0.7, 9.4);

	ASSERT(A.asMat4().asCFrame() == A);
	ASSERT(A.localToGlobal(v) == A.asMat4() * v);
	ASSERT(A.globalToLocal(v) == A.asMat4().inverse() * v);
	ASSERT(A4 * v == A4.asCFrame().localToGlobal(v));
	ASSERT(A4.inverse() * v == A4.asCFrame().globalToLocal(v));
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
	ASSERT((2 * v) % (3 * u) == 6 * (v % u));

	ASSERT(x % y == z);
	ASSERT(y % x == -z);
	ASSERT(y % z == x);
	ASSERT(z % y == -x);
	ASSERT(z % x == y);
	ASSERT(x % z == -y);
}

#include <iostream>
#include <algorithm>

TEST_CASE(eigenDecomposition) {
	for(double x = -1.25; x < 1.5; x += 0.1) {
		for(double y = -1.35; y < 1.5; y += 0.1) {
			for(double z = -1.55; z < 1.5; z += 0.1) {

				

				Mat3 orthoPart = fromEulerAngles(0.21, 0.31, 0.41);
				Mat3 eigenMat(x, 0, 0, 0, y, 0, 0, 0, z);
				Mat3 testMat = orthoPart * eigenMat * ~orthoPart;

				// std::cout << testMat;

				// std::cout << "doing" << x << ',' << y << ',' << z << '\n';

				EigenSet<double> v = testMat.getEigenDecomposition();

				ASSERT(EigenValues<double>(x, y, z) == v.eigenValues);
				ASSERT(v.eigenVectors * v.eigenValues.asDiagonalMatrix() * ~v.eigenVectors == testMat);


				// ASSERT()

				// std::cout << "Real: " << x << "," << y << ',' << z << " calculated: " << x << "," << y << ',' << z << '\n';

				// Log::debug("D0: %f, D1: %f, CrightPart: %f", D0, D1, CrightPart);

				//std::cout.precision(17);
				// std::cout << std::scientific;

				// if(v.eigenValues.v[0] > 0) std::cout <<std::fixed<< x << ','<<y<<','<<z<<'='<<std::scientific<<v.eigenValues.v[0] << '\n';
			}
		}
	}
	// ASSERT(false);
}
