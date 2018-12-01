#include "mat3.h"


Mat3 fromEulerAngles(double alpha, double beta, double gamma) {
	double sinA = sin(alpha), cosA = cos(alpha);
	double sinB = sin(beta), cosB = cos(beta);
	double sinC = sin(gamma), cosC = cos(gamma);
	return Mat3(
		cosC*cosB - sinC*sinA*sinB, -sinC*cosA, cosC*sinB + sinC*sinA*cosB,
		sinC*cosB + cosC*sinA*sinB, cosC*cosA, sinC*sinB - cosC*sinA*cosB,
		-cosA*sinB, sinA, cosA*cosB
	);
}

namespace Mat3Util {
	const Mat3 ZERO = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	const Mat3 IDENTITY = {1, 0, 0, 0, 1, 0, 0, 0, 1};
};
