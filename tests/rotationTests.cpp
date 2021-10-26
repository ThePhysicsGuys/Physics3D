#include "testsMain.h"

#include "compare.h"
#include "testValues.h"

#include <Physics3D/misc/toString.h>

#include <Physics3D/math/linalg/vec.h>
#include <Physics3D/math/linalg/quat.h>
#include <Physics3D/math/linalg/mat.h>
#include <Physics3D/math/linalg/trigonometry.h>
#include <Physics3D/math/rotation.h>
#include <Physics3D/math/linalg/commonMatrices.h>

#include <cmath>

using namespace P3D;
#define ASSERT(condition) ASSERT_TOLERANT(condition, 0.00000001)

#define FOR_XYZ(start, end, delta) for(auto x = start; x < end; x += delta) for(auto y = start; y < end; y += delta) for(auto z = start; z < end; z += delta) 

TEST_CASE(testFromRotationVecInvertsFromRotationMatrix) {
	FOR_XYZ(-1.55, 1.55, 0.13) {
		Vec3 v(x, y, z);
		Rotation rot = Rotation::fromRotationVec(v);

		Vec3 resultingVec = rot.asRotationVector();

		// logStream << "v = " << v << "\n  rot = " << rot << "  resultingVec = " << resultingVec;

		ASSERT(v == resultingVec);

		Rotation m = Rotation::fromEulerAngles(x, y, z);

		Vec3 rotVec = m.asRotationVector();

		Rotation resultingRot = Rotation::fromRotationVec(rotVec);

		// logStream << "m = " << m << "\n  rotVec = " << rotVec << "  resultingMat = " << resultingMat;

		ASSERT(m == resultingRot);
	}
}

TEST_CASE(rotationGlobalToLocalInvertsLocalToGlobal) {
	FOR_XYZ(-1.55, 1.55, 0.13) {
		Rotation rotToTest = Rotation::fromEulerAngles(x, y, z);

		Vec3 testVec(0.7, -0.5, 0.3);
		ASSERT(rotToTest.localToGlobal(rotToTest.globalToLocal(testVec)) == testVec);
		ASSERT(rotToTest.globalToLocal(rotToTest.localToGlobal(testVec)) == testVec);

		Rotation testRot = Rotation::fromEulerAngles(0.5, 0.4, 0.5);
		ASSERT(rotToTest.localToGlobal(rotToTest.globalToLocal(testRot)) == testRot);
		ASSERT(rotToTest.globalToLocal(rotToTest.localToGlobal(testRot)) == testRot);

		SymmetricMat3 testMat{
			0.7,
			0.3, 0.8,
			-0.2, 0.4, -1.0,
		};
		ASSERT(rotToTest.localToGlobal(rotToTest.globalToLocal(testMat)) == testMat);
		ASSERT(rotToTest.globalToLocal(rotToTest.localToGlobal(testMat)) == testMat);
	}
}

TEST_CASE(rotationGlobalToLocalMatrixIdentity) {
	FOR_XYZ(-1.55, 1.55, 0.13) {
		Rotation rotToTest = Rotation::fromEulerAngles(x, y, z);

		Vec3 testVec(0.7, -0.5, 0.3);
		SymmetricMat3 testMat{
			0.7,
			0.3, 0.8,
			-0.2, 0.4, -1.0,
		};

		ASSERT(rotToTest.localToGlobal(testMat * testVec) == rotToTest.localToGlobal(testMat) * rotToTest.localToGlobal(testVec));
		ASSERT(rotToTest.globalToLocal(testMat * testVec) == rotToTest.globalToLocal(testMat) * rotToTest.globalToLocal(testVec));
	}
}

TEST_CASE(rotationAssociative) {
	FOR_XYZ(-1.55, 1.55, 0.13) {
		Rotation rotToTest1 = Rotation::fromEulerAngles(x, y, z);
		Rotation rotToTest2 = Rotation::fromEulerAngles(-0.17, 0.26, -0.247);

		Vec3 testVec(0.7, -0.5, 0.3);
		Rotation testRot = Rotation::fromEulerAngles(0.5, 0.4, 0.5);
		SymmetricMat3 testMat{
			0.7,
			0.3, 0.8,
			-0.2, 0.4, -1.0,
		};

		ASSERT(rotToTest1.localToGlobal(rotToTest2.localToGlobal(testVec)) == rotToTest1.localToGlobal(rotToTest2).localToGlobal(testVec));

		ASSERT(rotToTest1.localToGlobal(rotToTest2.localToGlobal(testRot)) == rotToTest1.localToGlobal(rotToTest2).localToGlobal(testRot));

		ASSERT(rotToTest1.localToGlobal(rotToTest2.localToGlobal(testMat)) == rotToTest1.localToGlobal(rotToTest2).localToGlobal(testMat));
	}
}

TEST_CASE(rotXrotYrotZ) {
	FOR_XYZ(-1.5, 1.5, 0.1) {
		double angle = x;

		Vec2 a(y, z);
		Mat2 rotMat{
			std::cos(angle), -std::sin(angle),
			std::sin(angle), std::cos(angle)
		};
		Vec2 b = rotMat * a;

		double offset = 0.57;

		ASSERT(Rotation::rotX(angle) * Vec3(offset, a.x, a.y) == Vec3(offset, b.x, b.y));
		ASSERT(Rotation::rotY(angle) * Vec3(a.y, offset, a.x) == Vec3(b.y, offset, b.x));
		ASSERT(Rotation::rotZ(angle) * Vec3(a.x, a.y, offset) == Vec3(b.x, b.y, offset));
	}
}

TEST_CASE(faceDirection) {
	FOR_XYZ(-1.5, 1.5, 0.1) {
		if(x == 0.0 && y == 0.0 && z == 0.0) continue;
		Vec3 faceDirection = normalize(Vec3(x, y, z));

		ASSERT(Rotation::faceX(faceDirection) * Vec3(1.0, 0.0, 0.0) == faceDirection);
		ASSERT(Rotation::faceY(faceDirection) * Vec3(0.0, 1.0, 0.0) == faceDirection);
		ASSERT(Rotation::faceZ(faceDirection) * Vec3(0.0, 0.0, 1.0) == faceDirection);
	}
}

TEST_CASE(rotationMatrix) {
	FOR_XYZ(-1.55, 1.55, 0.1) {
		Mat3 mat = rotationMatrixfromEulerAngles(x, y, z);
		Rotation rot = Rotation::fromEulerAngles(x, y, z);
		ASSERT(rot == Rotation::fromRotationMatrix(mat));
		ASSERT(mat == rot.asRotationMatrix());
	}
}

TEST_CASE(rotationQuaternionNegative) {
	FOR_XYZ(-1.55, 1.55, 0.1) {
		QuaternionRotationTemplate<double> quatRot = QuaternionRotationTemplate<double>::fromEulerAngles(x, y, z);
		Quat4 q1 = quatRot.asRotationQuaternion();
		Quat4 q2 = -q1;

		QuaternionRotationTemplate<double> rq1 = QuaternionRotationTemplate<double>::fromRotationQuaternion(q1);
		QuaternionRotationTemplate<double> rq2 = QuaternionRotationTemplate<double>::fromRotationQuaternion(q2);

		ASSERT(rq1.asRotationVector() == rq2.asRotationVector());
		ASSERT(rq1.asRotationMatrix() == rq2.asRotationMatrix());
	}
}

TEST_CASE(quaternionFromRotVecInverse) {
	// 1.55 to try to stay below a length of PI, since otherwise duplicate vectors will emerge
	FOR_XYZ(-1.55, 1.55, 0.1) {
		Vec3 rotVec = Vec3(x, y, z);
		ASSERT(rotVec == rotationVectorFromRotationQuaternion(rotationQuaternionFromRotationVec(rotVec)));
	}
}

TEST_CASE(rotationImplementationIdenticalFromEulerAngles) {
	FOR_XYZ(1.55, 1.55, 0.1) {
		QuaternionRotationTemplate<double> quatRot = QuaternionRotationTemplate<double>::fromEulerAngles(x, y, z);
		MatrixRotationTemplate<double> matRot = MatrixRotationTemplate<double>::fromEulerAngles(x, y, z);

		ASSERT(quatRot.asRotationMatrix() == matRot.asRotationMatrix());
		ASSERT(quatRot.asRotationQuaternion() == matRot.asRotationQuaternion());
		ASSERT(quatRot.asRotationVector() == matRot.asRotationVector());
	}
}

TEST_CASE(rotationImplementationIdenticalLocalGlobal) {
	FOR_XYZ(1.5, 1.5, 0.5) {
		QuaternionRotationTemplate<double> quatRot = QuaternionRotationTemplate<double>::fromEulerAngles(x, y, z);
		MatrixRotationTemplate<double> matRot = MatrixRotationTemplate<double>::fromEulerAngles(x, y, z);

		FOR_XYZ(1.5, 1.5, 0.5) {
			Vec3 vec = Vec3(x, y, z);
			ASSERT(quatRot.localToGlobal(vec) == matRot.localToGlobal(vec));
			ASSERT(quatRot.globalToLocal(vec) == matRot.globalToLocal(vec));
		}
		FOR_XYZ(1.5, 1.5, 0.5) {
			QuaternionRotationTemplate<double> quatRot2 = QuaternionRotationTemplate<double>::fromEulerAngles(x, y, z);
			MatrixRotationTemplate<double> matRot2 = MatrixRotationTemplate<double>::fromEulerAngles(x, y, z);

			ASSERT(quatRot.localToGlobal(quatRot2).asRotationMatrix() == matRot.localToGlobal(matRot2).asRotationMatrix());
			ASSERT(quatRot.globalToLocal(quatRot2).asRotationMatrix() == matRot.globalToLocal(matRot2).asRotationMatrix());
			ASSERT((quatRot * quatRot2).asRotationMatrix() == (matRot * matRot2).asRotationMatrix());
		}
		SymmetricMat3 mat{
			1.3,
			0.4, -2.1,
			-1.2, 0.1, 0.7
		};

		ASSERT(quatRot.localToGlobal(mat) == matRot.localToGlobal(mat));
		ASSERT(quatRot.globalToLocal(mat) == matRot.globalToLocal(mat));
	}
}

TEST_CASE(rotationImplementationIdenticalInverse) {
	FOR_XYZ(1.5, 1.5, 0.5) {
		QuaternionRotationTemplate<double> quatRot = QuaternionRotationTemplate<double>::fromEulerAngles(x, y, z);
		MatrixRotationTemplate<double> matRot = MatrixRotationTemplate<double>::fromEulerAngles(x, y, z);

		ASSERT((~quatRot).asRotationMatrix() == (~matRot).asRotationMatrix());
	}
}

TEST_CASE(rotationImplementationIdenticalFaceMatrices) {
	FOR_XYZ(1.5, 1.5, 0.5) {
		Vec3 faceDir = Vec3(x, y, z);


		ASSERT(MatrixRotationTemplate<double>::faceX(faceDir).asRotationMatrix() == QuaternionRotationTemplate<double>::faceX(faceDir).asRotationMatrix());
		ASSERT(MatrixRotationTemplate<double>::faceY(faceDir).asRotationMatrix() == QuaternionRotationTemplate<double>::faceY(faceDir).asRotationMatrix());
		ASSERT(MatrixRotationTemplate<double>::faceZ(faceDir).asRotationMatrix() == QuaternionRotationTemplate<double>::faceZ(faceDir).asRotationMatrix());
	}
}

TEST_CASE(rotationImplementationIdenticalGetXYZ) {
	FOR_XYZ(1.5, 1.5, 0.5) {
		MatrixRotationTemplate<double> matRot = MatrixRotationTemplate<double>::fromEulerAngles(x, y, z);
		QuaternionRotationTemplate<double> quatRot = QuaternionRotationTemplate<double>::fromEulerAngles(x, y, z);

		ASSERT(matRot.getX() == quatRot.getX());
		ASSERT(matRot.getY() == quatRot.getY());
		ASSERT(matRot.getZ() == quatRot.getZ());

		ASSERT(matRot.getX() == matRot.localToGlobal(Vec3(1, 0, 0)));
		ASSERT(matRot.getY() == matRot.localToGlobal(Vec3(0, 1, 0)));
		ASSERT(matRot.getZ() == matRot.localToGlobal(Vec3(0, 0, 1)));

		ASSERT(quatRot.getX() == quatRot.localToGlobal(Vec3(1, 0, 0)));
		ASSERT(quatRot.getY() == quatRot.localToGlobal(Vec3(0, 1, 0)));
		ASSERT(quatRot.getZ() == quatRot.localToGlobal(Vec3(0, 0, 1)));
	}
}
