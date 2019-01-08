#include "testsMain.h"

#include "../engine/math/mat3.h"
#include "../engine/math/mat4.h"
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
	ASSERT((2 * v) % (3 * u) == 6 * (v%u));

	ASSERT(x % y == z);
	ASSERT(y % x == -z);
	ASSERT(y % z == x);
	ASSERT(z % y == -x);
	ASSERT(z % x == y);
	ASSERT(x % z == -y);
}
