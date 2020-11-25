#include "testsMain.h"

#include "compare.h"
#include "testValues.h"
#include "randomValues.h"
#include "simulation.h"
#include "../physics/misc/toString.h"
#include "../physics/math/linalg/vec.h"
#include "../physics/math/linalg/quat.h"
#include "../physics/math/linalg/mat.h"
#include "../physics/math/linalg/trigonometry.h"
#include "../physics/math/linalg/largeMatrix.h"
#include "../physics/math/linalg/largeMatrixAlgorithms.h"
#include "../physics/math/linalg/eigen.h"
#include "../physics/math/mathUtil.h"
#include "../physics/math/taylorExpansion.h"
#include "../physics/math/predefinedTaylorExpansions.h"
#include "../physics/math/linalg/commonMatrices.h"

#include <algorithm>


#define ASSERT(condition) ASSERT_TOLERANT(condition, 0.00000001)

#define DELTA_T 0.00001

TEST_CASE(subMatrixOperations) {
	Matrix<int, 5, 3> mat{
		5,7,9,
		6,9,2,
		3,9,5,
		3,8,4,
		3,8,1
	};

	ASSERT((mat.getSubMatrix<3, 2>(1, 0)) == (Matrix<int, 3, 2>{
		6, 9,
		3, 9,
		3, 8
	}));

	ASSERT((mat.withoutRow(2)) == (Matrix<int, 4, 3>{
		5, 7, 9,
		6, 9, 2,
		3, 8, 4,
		3, 8, 1
	}));

	ASSERT((mat.withoutCol(1)) == (Matrix<int, 5, 2>{
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
	CFrame A(Vec3(1.0, 0.8, 0.9), Rotation::fromEulerAngles(0.3, 0.7, 0.9));
	CFrame B(Vec3(8.2, -0.8, 3.4), Rotation::fromEulerAngles(0.4, 0.4, 0.3));

	ASSERT(A.localToGlobal(A.globalToLocal(B)) == B);
	ASSERT(A.globalToLocal(A.localToGlobal(B)) == B);
	ASSERT(A.globalToLocal(A) == CFrame());
	ASSERT(A.localToGlobal(~A) == CFrame());
	ASSERT((~A).localToGlobal(A) == CFrame());

	ASSERT(A.localToRelative(B) + A.position == A.localToGlobal(B));
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

	CFrame A(Vec3(0.7, 1.02, 0.9), Rotation::fromEulerAngles(0.7, 0.9, 0.3));
	CFrame B(Vec3(-0.2, 10.02, 0.3), Rotation::fromEulerAngles(0.2, 0.2, 0.1));
	CFrame C(Vec3(-0.2343657, 17.02, -9.3), Rotation::fromEulerAngles(0.9, 0.4, 0.9));

	Vec3 v(17, -0.7, 9.4);

	ASSERT(A.localToGlobal(B).localToGlobal(C) == A.localToGlobal(B.localToGlobal(C)));
	ASSERT(A.localToGlobal(B).localToGlobal(v) == A.localToGlobal(B).localToGlobal(v));
}

TEST_CASE(fromEuler) {
	ASSERT(rotMatZ(0.5)*rotMatX(0.3)*rotMatY(0.7) == rotationMatrixfromEulerAngles(0.3, 0.7, 0.5));
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

TEST_CASE(eigenDecomposition) {
	for(double x = -1.25; x < 1.5; x += 0.1) {
		for(double y = -1.35; y < 1.5; y += 0.1) {
			for(double z = -1.55; z < 1.5; z += 0.1) {
				Mat3 orthoPart = rotationMatrixfromEulerAngles(0.21, 0.31, 0.41);
				DiagonalMat3 eigenMat{x,y,z};

				SymmetricMat3 testMat = mulSymmetricLeftRightTranspose(SymmetricMat3(eigenMat), orthoPart);

				EigenSet<double, 3> v = getEigenDecomposition(testMat);

				EigenValues<double, 3> realValues{x,y,z};

				ASSERT(realValues == v.eigenValues);
				ASSERT(transformBasis(v.eigenValues.asDiagonalMatrix(), v.eigenVectors) == testMat);
			}
		}
	}
}

TEST_CASE(largeMatrixVectorSolve) {
	LargeMatrix<double> mat(5, 5);
	LargeVector<double> vec(5);

	for (int i = 0; i < 5; i++) {
		vec[i] = fRand(-1.0, 1.0);
	}

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			mat(i,j) = fRand(-1.0, 1.0);
		}
	}

	mat(0,0) = 0;

	LargeVector<double> newVector = mat * vec;

	LargeVector<double> solutionVector = newVector;

	destructiveSolve(mat, solutionVector);

	ASSERT(solutionVector == vec);
}

TEST_CASE(testTaylorExpansion) {
	FullTaylorExpansion<double, 5> testTaylor{2.0, 5.0, 2.0, 3.0, -0.7};

	double x = 0.47;

	double correctResult = 
		testTaylor.getConstantValue() + 
		testTaylor.getDerivative(0) * x + 
		testTaylor.getDerivative(1) * x * x / 2 + 
		testTaylor.getDerivative(2) * x * x * x / 6 + 
		testTaylor.getDerivative(3) * x * x * x * x / 24;

	ASSERT(correctResult == testTaylor(x));
}

TEST_CASE(testSinTaylorExpansion) {
	double curAngle = 0.93;
	double multiplier = 13.97;
	// Taylor expansion for sin(x * multiplier) at x=curAngle
	FullTaylorExpansion<double, 5> testTaylor = generateFullTaylorForSinWave<double, 5>(curAngle, multiplier);

	double constTerm = sin(curAngle * multiplier);
	double firstDerivative = multiplier * cos(curAngle * multiplier);
	double secondDerivative = multiplier * multiplier * (-sin(curAngle * multiplier));
	double thirdDerivative = multiplier * multiplier * multiplier * (-cos(curAngle * multiplier));
	double fourthDerivative = multiplier * multiplier * multiplier * multiplier * sin(curAngle * multiplier);

	ASSERT(testTaylor.getConstantValue() == constTerm);
	ASSERT(testTaylor.getDerivative(0) == firstDerivative);
	ASSERT(testTaylor.getDerivative(1) == secondDerivative);
	ASSERT(testTaylor.getDerivative(2) == thirdDerivative);
	ASSERT(testTaylor.getDerivative(3) == fourthDerivative);
}

TEST_CASE(testMultiplicationDerivatives) {
	constexpr int COUNT = 4;
	FullTaylorExpansion<Mat3, COUNT> mats = createRandomFullTaylorExpansion<Mat3, COUNT, createRandomMatrixTemplate<double, 3, 3>>();
	FullTaylorExpansion<Vec3, COUNT> vecs = createRandomFullTaylorExpansion<Vec3, COUNT, createRandomVecTemplate<double, 3>>();

	FullTaylorExpansion<Vec3, COUNT> resultToTest = derivsOfMultiplication(mats, vecs);

	std::array<Mat3, COUNT> matVals = computeOverTime(mats, DELTA_T);
	std::array<Vec3, COUNT> vecVals = computeOverTime(vecs, DELTA_T);

	std::array<Vec3, COUNT> targetResults;
	for(int i = 0; i < COUNT; i++) targetResults[i] = matVals[i] * vecVals[i];

	FullTaylorExpansion<Vec3, COUNT> estimatedRealDerivatives = estimateDerivatives(targetResults, DELTA_T);

	logStream << "resultToTest: " << resultToTest << "\n";
	logStream << "estimatedRealDerivatives: " << estimatedRealDerivatives << "\n";

	for(int i = 0; i < COUNT; i++) {
		double tolerance = 0.0000001 * std::exp(10.0*i);
		logStream << "tolerance: " << tolerance << "\n";
		ASSERT_TOLERANT(resultToTest.derivs[i] == estimatedRealDerivatives.derivs[i], tolerance);
	}
}

TEST_CASE(testCrossProductEquivalent) {
	Vec3 v(1.3, -0.5, 0.7);
	Vec3 x(0.6, 0.5, -1.2);

	ASSERT(v % x == createCrossProductEquivalent(v) * x);
}

TEST_CASE(testSkewSymmetricValid) {
	Vec3 v(1.3, -0.5, 0.7);
	Vec3 x(0.6, 0.5, -1.2);

	ASSERT((v % (v % x)) == skewSymmetricSquared(v) * x);
	Mat3 sv = createCrossProductEquivalent(v);
	ASSERT(sv * sv == Mat3(skewSymmetricSquared(v)));
}

TEST_CASE(testSkewSymmetricDerivatives) {
	Vec3 start(1.3, 1.7, -0.5);
	Vec3 deriv1(-0.2, 0.4, -0.5);
	Vec3 deriv2(-0.3, 0.2, -0.1);

	FullTaylor<Vec3> vecInput{start, deriv1, deriv2};

	FullTaylor<SymmetricMat3> skewSymOut = generateFullTaylorForSkewSymmetricSquared<double, 3>(vecInput);

	SymmetricMat3 s0 = skewSymmetricSquared(vecInput(0.0));
	SymmetricMat3 s1 = skewSymmetricSquared(vecInput(DELTA_T));
	SymmetricMat3 s2 = skewSymmetricSquared(vecInput(DELTA_T * 2.0));

	ASSERT_TOLERANT(s0 == skewSymOut.getConstantValue(), 0.000000001);
	ASSERT_TOLERANT((s1 - s0) / DELTA_T == skewSymOut.getDerivative(0), 0.00005);
	ASSERT_TOLERANT((s2 + s0 - 2.0*s1) / (DELTA_T * DELTA_T) == skewSymOut.getDerivative(1), 0.00005);
}

TEST_CASE(testSimulation) {
	FullTaylorExpansion<Vec3, 3> realDerivs{Vec3(0.4, -0.6, 0.7), Vec3(-0.2, 0.4, -0.8), Vec3(0.5, -0.35, 0.7)};

	double deltaT = 0.000001;

	std::array<Vec3, 3> computedPoints = computeTranslationOverTime(realDerivs.getConstantValue(), realDerivs.getDerivatives(), deltaT);

	FullTaylorExpansion<Vec3, 3> computedDerivs = estimateDerivatives(computedPoints, deltaT);

	ASSERT_TOLERANT(realDerivs == computedDerivs, 0.0005);
}

TEST_CASE(testRotationDerivs) {
	Mat3 rot0 = rotationMatrixfromEulerAngles(0.3, -0.5, 0.7);
	for(Vec3 angularVel : vectors) {
		for(Vec3 angularAccel : vectors) {
			logStream << "angularVel: " << angularVel << " accel: " << angularAccel << "\n";
			TaylorExpansion<Vec3, 2> rotVecTaylor{angularVel, angularAccel};
			FullTaylorExpansion<Mat3, 3> rotTaylor = FullTaylorExpansion<Mat3, 3>::fromConstantAndDerivatives(rot0, generateTaylorForRotationMatrix<double, 2>(rotVecTaylor, rot0));
			
			Vec3 rotVec1 = rotVecTaylor(DELTA_T);
			Vec3 rotVec2 = rotVecTaylor(DELTA_T*2);
			Mat3 deltaRot1 = rotationMatrixFromRotationVec(rotVec1);
			Mat3 deltaRot2 = rotationMatrixFromRotationVec(rotVec2);
			Mat3 rot1 = deltaRot1 * rot0;
			Mat3 rot2 = deltaRot2 * rot0;
			
			ASSERT_TOLERANT((rot1 - rot0) / DELTA_T == rotTaylor.getDerivative(0), 0.0005 * lengthSquared(angularVel) + lengthSquared(angularAccel));
			ASSERT_TOLERANT((rot2 + rot0 - 2.0 * rot1) / (DELTA_T * DELTA_T) == rotTaylor.getDerivative(1), 0.05 * (lengthSquared(angularVel) + lengthSquared(angularAccel)));
		}
	}
}

TEST_CASE(testFromRotationVec) {
	double deltaT = 0.0001;

	Vec3 rotations[]{Vec3(1,0,0), Vec3(0,1,0), Vec3(0,0,1), Vec3(-5.0, 3.0, 9.0), Vec3(3.8, -0.5, 0.4)};
	for(Vec3 rotVec : rotations) {
		Vec3 rotationVec = rotVec * deltaT;
		Vec3 pointToRotate = Vec3(1.0, 0.0, 0.0);

		Mat3 rot = rotationMatrixFromRotationVec(rotationVec);

		Vec3 rotateByRotVec = pointToRotate;
		Vec3 rotateByRotMat = pointToRotate;

		for(int i = 0; i < 500; i++) {
			rotateByRotVec = rotateByRotVec + rotationVec % rotateByRotVec;
			rotateByRotMat = rot * rotateByRotMat;
		}

		logStream << "rotVec: " << rotationVec << ", rot: " << rot << '\n';
		logStream << "newPointRotVec: " << rotateByRotVec << "\nnewPointRotMat: " << rotateByRotMat << '\n';

		ASSERT_TOLERANT(rotateByRotVec == rotateByRotMat, 0.01);
	}
}

// Everything Quaternions
TEST_CASE(quaternionBasicRotation) {
	Vec3 v = Vec3(0.0, 1.0, 0.0);
	Quat4 q = QROT_X_180(double);

	Quat4 r = q * v * conj(q);

	ASSERT(r.w == 0.0);
	ASSERT(r.v == Vec3(0.0, -1.0, 0.0));
}

TEST_CASE(quaternionRotationShorthand) {
	Vec3 v = Vec3(0.7, 1.0, 0.3);
	Quat4 q = normalize(Quat4(0.8, 0.4, 0.3, -0.4));

	ASSERT(Quat4(0.0, mulQuaternionLeftRightConj(q, v)) == q * v * conj(q));
	ASSERT(Quat4(0.0, mulQuaternionLeftConjRight(q, v)) == conj(q) * v * q);
}

TEST_CASE(quaternionFromRotationVecToRotationMatrix) {
	for(double x = -1.25; x < 1.5; x += 0.1) {
		for(double y = -1.35; y < 1.5; y += 0.1) {
			for(double z = -1.55; z < 1.5; z += 0.1) {
				Vec3 rotVec(x,y,z);
				ASSERT(rotationMatrixFromQuaternion(rotationQuaternionFromRotationVec(rotVec)) == rotationMatrixFromRotationVec(rotVec));
			}
		}
	}
}

TEST_CASE(interchanceQuaternionAndMatrix){
	for(double x = -1.25; x < 1.5; x += 0.1) {
		for(double y = -1.35; y < 1.5; y += 0.1) {
			for(double z = -1.55; z < 1.5; z += 0.1) {
				Mat3 rotMat = rotationMatrixfromEulerAngles(x, y, z);
				Quat4 rotQuat = rotationQuaternionFromRotationMatrix(rotMat);
				Mat3 resultMat = rotationMatrixFromQuaternion(rotQuat);

				ASSERT(rotMat == resultMat);
			}
		}
	}
}

TEST_CASE(testFixCastToInt) {
	for(int i = 0; i < 10000; i++) {
		int64_t a = rand() - RAND_MAX/2;
		Fix<32> f(a);
		logStream << a << '\n';
		ASSERT_STRICT(static_cast<int64_t>(f) == a);
	}
	for(int i = 0; i < 10000; i++) {
		double d = double(rand()) / RAND_MAX * 100000.0;
		Fix<32> f(d);
		logStream << d << '\n';

		ASSERT_STRICT(static_cast<int64_t>(f) == static_cast<int64_t>(d));
	}
}
