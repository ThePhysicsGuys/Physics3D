#include "mat3.h"

#include <cmath>

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

template<typename N>
Mat3Template<N> Mat3Template<N>::rotate(N angle, N x, N y, N z) const {
	N s = sin(angle);
	N c = cos(angle);
	N C = 1 - c;
	N rm00 = x * x * C + c;
	N rm01 = x * y * C + z * s;
	N rm02 = x * z * C - y * s;
	N rm10 = x * y * C - z * s;
	N rm11 = y * y * C + c;
	N rm12 = y * z * C + x * s;
	N rm20 = x * z * C + y * s;
	N rm21 = y * z * C - x * s;
	N rm22 = z * z * C + c;
	N r00 = m00 * rm00 + m10 * rm01 + m20 * rm02;
	N r01 = m01 * rm00 + m11 * rm01 + m21 * rm02;
	N r02 = m02 * rm00 + m12 * rm01 + m22 * rm02;
	N r10 = m00 * rm10 + m10 * rm11 + m20 * rm12;
	N r11 = m01 * rm10 + m11 * rm11 + m21 * rm12;
	N r12 = m02 * rm10 + m12 * rm11 + m22 * rm12;
	N r20 = m00 * rm20 + m10 * rm21 + m20 * rm22;
	N r21 = m01 * rm20 + m11 * rm21 + m21 * rm22;
	N r22 = m02 * rm20 + m12 * rm21 + m22 * rm22;

	return Mat3Template<N>(r00, r01, r02, r10, r11, r12, r20, r21, r22);
}

template<typename N>
Mat3Template<N> fromRotationVec(Vec3Template<N> rotVec) {
	
	N angle = rotVec.length();

	if (!(angle != 0)) { // inverse is important! Catches weird values like Nan and Inf too
		return Mat3Template<N>(1, 0, 0,
							   0, 1, 0,
							   0, 0, 1);
	}

	rotVec /= angle;
	N x = rotVec.x;
	N y = rotVec.y;
	N z = rotVec.z;

	N sinA = sin(angle);
	N cosA = cos(angle);

	Mat3Template<N> outer = rotVec.outer(rotVec);
	Mat3Template<N> rotor = Mat3Template<N>(cosA,     z*sinA,   -y*sinA,
											-z*sinA,  cosA,     x*sinA,
											y*sinA,   -x*sinA,  cosA);

	return outer * (1 - cosA) + rotor;
}

template class Mat3Template<double>;
template class Mat3Template<float>;

template Mat3Template<double> fromRotationVec(Vec3Template<double> v);
template Mat3Template<float> fromRotationVec(Vec3Template<float> v);
