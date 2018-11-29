#pragma once

#include <cmath>
#include <string>
#include <sstream>

#include "vec3.h"

template<typename N>
class Mat3Template {
public:
	union {
		struct {
			N m00;
			N m01;
			N m02;
			N m10;
			N m11;
			N m12;
			N m20;
			N m21;
			N m22;
		};

		N m[9];
	};

	Mat3Template(N m00, N m01, N m02, N m10, N m11, N m12, N m20, N m21, N m22) :
		m00(m00), m01(m01), m02(m02), m10(m10), m11(m11), m12(m12), m20(m20), m21(m21), m22(m22) {};

	Mat3Template(const N other[9]) {
		for (int i = 0; i < 9; i++) {
			m[i] = other[i];
		}
	};

	Mat3Template(const Mat3Template& other) {
		for (int i = 0; i < 9; i++) {
			m[i] = other.m[i];
		}
	}

	N det() {
		return m00 * (m11 * m22 - m12 * m21) - m01 * (m10 * m22 - m12 * m20) + m02 * (m10 * m21 - m11 * m20);
	}

	Mat3Template inverse() {
		N normdet = det();
		invdet = 1 / normdet;
		N r00 = (m11 * m22 - m21 * m12) * invdet;
		N r01 = (m21 * m02 - m01 * m22) * invdet;
		N r02 = (m01 * m12 - m11 * m02) * invdet;
		N r10 = (m20 * m12 - m10 * m22) * invdet;
		N r11 = (m00 * m22 - m20 * m02) * invdet;
		N r12 = (m10 * m02 - m00 * m12) * invdet;
		N r20 = (m10 * m21 - m20 * m11) * invdet;
		N r21 = (m20 * m01 - m00 * m21) * invdet;
		N r22 = (m00 * m11 - m10 * m01) * invdet;
		
		return Mat3Template(m00, m01, m02, m10, m11, m12, m20, m21, m22)
	}

	Mat3Template rotate(N angle, N x, N y, N z) {
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

		return Mat3Template(r00, r01, r02, r10, r11, r12, r20, r21, r22)
	}

	Mat3Template transpose() {
		return Mat3Template(m00, m10, m20, m01, m11, m21, m02, m12, m22);
	}

	Mat3Template operator+(const Mat3Template& other) const {
		return Mat3Template(
			m00 + other.m00,
			m01 + other.m01,
			m02 + other.m02,
			m10 + other.m10,
			m11 + other.m11,
			m12 + other.m12,
			m20 + other.m20,
			m21 + other.m21,
			m22 + other.m22
		);
	}

	Mat3Template operator-(const Mat3Template& other) const {
		return Mat3Template (
			m00 - other.m00,
			m01 - other.m01,
			m02 - other.m02,
			m10 - other.m10,
			m11 - other.m11,
			m12 - other.m12,
			m20 - other.m20,
			m21 - other.m21,
			m22 - other.m22
		);
	}

	Mat3Template operator-() const {
		return Mat3Template(-m00, -m01, -m02, -m10, -m11, -m12, -m20, -m21, -m22);
	}

	Vec3Template operator*(const Vec3Template& other) const {
		return Vec3(
			m00 * other.x + m01 * other.y + m02 * other.z, 
			m10 * other.x + m11 * other.y + m12 * other.z, 
			m20 * other.x + m21 * other.y + m22 * other.z
		);
	}

	Mat3Template operator*(const Mat3Template& other) const {
		N r00 = m00 * other.m00 + m10 * other.m01 + m20 * other.m02;
		N r01 = m01 * other.m00 + m11 * other.m01 + m21 * other.m02;
		N r02 = m02 * other.m00 + m12 * other.m01 + m22 * other.m02;
		N r10 = m00 * other.m10 + m10 * other.m11 + m20 * other.m12;
		N r11 = m01 * other.m10 + m11 * other.m11 + m21 * other.m12;
		N r12 = m02 * other.m10 + m12 * other.m11 + m22 * other.m12;
		N r20 = m00 * other.m20 + m10 * other.m21 + m20 * other.m22;
		N r21 = m01 * other.m20 + m11 * other.m21 + m21 * other.m22;
		N r22 = m02 * other.m20 + m12 * other.m21 + m22 * other.m22;

		return Mat3Template(r00, r01, r02, r10, r11, r12, r20, r21, r22);
	}

	friend std::ostream& operator<<(std::ostream& os, const Mat3Template& matrix) const {
		os << "Mat3Template(";

		for (int i = 0; i < 8; i++) {
			os << matrix.m[i];
			os << (((i + 1) % 4 == 0) ? "; " : ", ");
		}
		os << matrix.m[8] << ")";

		return os;
	}

	std::string str() {
		std::stringstream ss;
		ss << std::string("Mat3Template(");

		for (int i = 0; i < 8; i++) {
			ss << m[i];
			ss << (((i + 1) % 4 == 0) ? "; " : ", ");
		}
		ss << m[8] << ")";

		return ss.str();
	}

};

typedef Mat3Template<double>	Mat3;
typedef Mat3Template<float>		Mat3f;
typedef Mat3Template<long long>	Mat3l;

namespace Mat3TemplateUtil {
	Mat3 ZERO = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	Mat3 IDENTITY = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
};