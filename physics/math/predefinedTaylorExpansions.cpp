#include "predefinedTaylorExpansions.h"

#define TOO_SMALL_ANGLE_CUTOFF 1.0e-20

/*
	Computes the full taylor expansion for rotationMatrixfromRotationVec
*/
template<int Derivs>
TaylorExpansion<Mat3, Derivs> generateTaylorForRotationMatrixFromRotationVector(const FullTaylorExpansion<Vec3, Vec3, Derivs>& rotationVecTaylor) {
	static_assert(Derivs >= 1 && Derivs <= 2);

	Vec3 f = rotationVecTaylor.constantValue;
	Vec3 ff = rotationVecTaylor.derivatives[0];

	double angleSq = lengthSquared(rotationVecTaylor.constantValue);
	double angle = sqrt(angleSq);

	double cosAngle = cos(angle);

	// sinc(angle) = sin(angle) / angle
	// around angle=0 =~ 1 - angle^2 / 6 + angle^4 / 120 - angle^6 / 5040 
	double sincAngle = (angleSq > TOO_SMALL_ANGLE_CUTOFF) ? sin(angle) / angle : 1.0 - angleSq / 6.0; 

	// cosc(angle) = (cos(angle) - 1) / angle^2
	// around angle=0 =~ 1/2 - angle^2 / 24 + angle^4 / 720 - angle^6 / 40320
	double coscAngle = (angleSq > TOO_SMALL_ANGLE_CUTOFF) ? (1 - cosAngle) / angleSq : 0.5 - angleSq / 24.0;

	TaylorExpansion<Mat3, Derivs> result;

	// sinc2cosc(x) = (sinc(x) - 2 * cosc(x)) / x^2
	// around angle=0 =~ -1/12 + x^2/180
	double sinc2coscAngle = (angleSq > TOO_SMALL_ANGLE_CUTOFF) ? (sincAngle - 2 * coscAngle) / angleSq : -1.0/12.0 + angleSq / 180.0;

	// sincNegCos(angle) = (sinc(angle) - cos(angle)) / angle^2
	// around angle=0 =~ 1/3 - angle^2/30 + angle^4/840
	double sincNegCosAngle = (angleSq > TOO_SMALL_ANGLE_CUTOFF) ? (sincAngle - cosAngle) / angleSq : 1.0 / 3.0 - angleSq / 30;

	double selfDot = f * ff;

	Vec3 fSquared = elementWiseSquare(f);

	// First derivative
	{
		double diag = -sincAngle * selfDot;
		Vec3 offDiag = sincNegCosAngle * selfDot * f - sincAngle * ff;

		Mat3 rotorDiff{
			diag, offDiag.z, -offDiag.y,
			-offDiag.z, diag, offDiag.x,
			offDiag.y, -offDiag.x, diag
		};

		Vec3 diagElems = selfDot * sinc2coscAngle * fSquared + elementWiseMul(f, ff) * (2.0 * coscAngle);
		Vec3 offDiagElems = selfDot * sinc2coscAngle * mulSelfOpposites(f) + mulOppositesBiDir(f, ff) * coscAngle;

		SymmetricMat3 outerDiff{
			diagElems.x,
			offDiagElems.z, diagElems.y,
			offDiagElems.y, offDiagElems.x, diagElems.z
		};
		result.derivatives[0] = Mat3(outerDiff) + rotorDiff;
	}

	if constexpr(Derivs >= 2) {
		Vec3 fff = rotationVecTaylor.derivatives[1];

		// sincNegSincNegCos(angle) = (sinc(angle) - 3.0 * sincNegCos(angle))/angle^2
		// around angle=0 =~ -1/15 + angle^2 / 210
		double sincNegSincNegCosAngle = (angleSq > TOO_SMALL_ANGLE_CUTOFF) ? (sincAngle - 3.0 * sincNegCosAngle) / angleSq : -1.0/15.0 + angleSq / 210.0;

		// cosincosc(angle) = cos(angle) - 5.0 * sinc(angle) + 8.0 * cosc(angle)
		// around angle=0 =~ 1/90 - angle^2 / 1680 
		double cosincosc = (angleSq > TOO_SMALL_ANGLE_CUTOFF) ? (cosAngle - 5.0 * sincAngle + 8.0 * coscAngle) / (angleSq * angleSq) : 1.0 / 90.0 - angleSq / 1680.0;

		// Second derivative
		
		double selfDotSq = selfDot * selfDot;
		double selfDotDeriv = ff * ff + f * fff;

		double offDiagRotorF = sincNegCosAngle * selfDotDeriv + sincNegSincNegCosAngle * selfDotSq;
		double offDiagRotorFF = 2.0 * sincNegCosAngle * selfDot;
		double offDiagRotorFFF = -sincAngle;

		double diagRotor = sincNegCosAngle * selfDotSq - sincAngle * selfDotDeriv;
		Vec3 offDiagRotor = offDiagRotorF * f + offDiagRotorFF * ff + offDiagRotorFFF * fff;

		Mat3 rotorDiff2{
			diagRotor, offDiagRotor.z, -offDiagRotor.y,
			-offDiagRotor.z, diagRotor, offDiagRotor.x,
			offDiagRotor.y, -offDiagRotor.x, diagRotor
		};

		double oppF_FF = 2.0 * sinc2coscAngle * selfDot;
		double selfOppF = cosincosc * selfDotSq + sinc2coscAngle * selfDotDeriv;
		Vec3 doubleDeriv = (mulSelfOpposites(ff) * 2.0 + mulOppositesBiDir(f, fff)) * coscAngle;
		Vec3 offDiagOuter = mulOppositesBiDir(f, ff) * oppF_FF + doubleDeriv + mulSelfOpposites(f) * selfOppF;

		double fSqTerm = sinc2coscAngle * selfDotDeriv + cosincosc * selfDotSq;
		double mulF_FFTerm = 4.0 * sinc2coscAngle * selfDot;
		double doubleDerivOuter = 2.0 * coscAngle;

		Vec3 diagOuter = fSqTerm * fSquared + doubleDerivOuter * (elementWiseSquare(ff) + elementWiseMul(f, fff)) + mulF_FFTerm * elementWiseMul(f, ff);

		SymmetricMat3 outerDiff2{
			diagOuter.x,
			offDiagOuter.z, diagOuter.y,
			offDiagOuter.y, offDiagOuter.x, diagOuter.z
		};

		result.derivatives[1] = Mat3(outerDiff2) + rotorDiff2;
	}
	return result;
}

template TaylorExpansion<Mat3, 1> generateTaylorForRotationMatrixFromRotationVector<1>(const FullTaylorExpansion<Vec3, Vec3, 1>&);
template TaylorExpansion<Mat3, 2> generateTaylorForRotationMatrixFromRotationVector<2>(const FullTaylorExpansion<Vec3, Vec3, 2>&);

