#pragma once

#include "vec3.h"

template<typename N>
struct Mat3Template {
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

	N det() const {
		return m00 * (m11 * m22 - m12 * m21) - m01 * (m10 * m22 - m12 * m20) + m02 * (m10 * m21 - m11 * m20);
	}

	Mat3Template<N> inverse() const {
		N normdet = det();
		N invdet = 1 / normdet;
		N r00 = (m11 * m22 - m21 * m12) * invdet;
		N r01 = (m21 * m02 - m01 * m22) * invdet;
		N r02 = (m01 * m12 - m11 * m02) * invdet;
		N r10 = (m20 * m12 - m10 * m22) * invdet;
		N r11 = (m00 * m22 - m20 * m02) * invdet;
		N r12 = (m10 * m02 - m00 * m12) * invdet;
		N r20 = (m10 * m21 - m20 * m11) * invdet;
		N r21 = (m20 * m01 - m00 * m21) * invdet;
		N r22 = (m00 * m11 - m10 * m01) * invdet;
		
		return Mat3Template(m00, m01, m02, m10, m11, m12, m20, m21, m22);
	}

	Mat3Template<N> operator~() const {
		return inverse();
	}

	Mat3Template rotate(N angle, N x, N y, N z) const;

	Mat3Template transpose() const {
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

	Vec3Template<N> operator*(const Vec3Template<N>& other) const {
		return Vec3Template<N>(
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
};

typedef Mat3Template<double>	Mat3;
typedef Mat3Template<float>		Mat3f;
typedef Mat3Template<long long>	Mat3l;

typedef Mat3 RotMat3;

Mat3 fromEulerAngles(double alpha, double beta, double gamma);
Mat3 fromRotationVec(Vec3 rotVec);
