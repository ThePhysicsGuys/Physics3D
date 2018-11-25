#pragma once
#include <cmath>
#include <sstream>

class Mat4 {

public:
	union {
		struct {
			double m00;
			double m01;
			double m02;
			double m03;
			double m10;
			double m11;
			double m12;
			double m13;
			double m20;
			double m21;
			double m22;
			double m23;
			double m30;
			double m31;
			double m32;
			double m33;
		};

		double m[16];
	};

	Mat4(double m00, double m01, double m02, double m03, double m10, double m11, double m12, double m13, double m20, double m21, double m22, double m23, double m30, double m31, double m32, double m33) : 
		m00(m00), m01(m01), m02(m02), m03(m03), m10(m10), m11(m11), m12(m12), m13(m13), m20(m20), m21(m21), m22(m22), m23(m23), m30(m30), m31(m31), m32(m32), m33(m33) {};

	Mat4(const double other[16]) {
		for (int i = 0; i < 16; i++) {
			m[i] = other[i];
		}
	};

	Mat4(const Mat4& other) {
		for (int i = 0; i < 16; i++) {
			m[i] = other.m[i];
		}
	}

	double det() {
		double a = m00 * m11 - m01 * m10;
		double b = m00 * m12 - m02 * m10;
		double c = m00 * m13 - m03 * m10;
		double d = m01 * m12 - m02 * m11;
		double e = m01 * m13 - m03 * m11;
		double f = m02 * m13 - m03 * m12;
		double g = m20 * m31 - m21 * m30;
		double h = m20 * m32 - m22 * m30;
		double i = m20 * m33 - m23 * m30;
		double j = m21 * m32 - m22 * m31;
		double k = m21 * m33 - m23 * m31;
		double l = m22 * m33 - m23 * m32;
		double det = a * l - b * k + c * j + d * i - e * h + f * g;

		return det;
	}

	Mat4 inverse() {
		double a = m00 * m11 - m01 * m10;
		double b = m00 * m12 - m02 * m10;
		double c = m00 * m13 - m03 * m10;
		double d = m01 * m12 - m02 * m11;
		double e = m01 * m13 - m03 * m11;
		double f = m02 * m13 - m03 * m12;
		double g = m20 * m31 - m21 * m30;
		double h = m20 * m32 - m22 * m30;
		double i = m20 * m33 - m23 * m30;
		double j = m21 * m32 - m22 * m31;
		double k = m21 * m33 - m23 * m31;
		double l = m22 * m33 - m23 * m32;
		double normdet = a * l - b * k + c * j + d * i - e * h + f * g;

		double invdet = 1.0 / normdet;

		double r00 = ( m11 * l - m12 * k + m13 * j) * invdet;
		double r01 = (-m01 * l + m02 * k - m03 * j) * invdet;
		double r02 = ( m31 * f - m32 * e + m33 * d) * invdet;
		double r03 = (-m21 * f + m22 * e - m23 * d) * invdet;
		double r10 = (-m10 * l + m12 * i - m13 * h) * invdet;
		double r11 = ( m00 * l - m02 * i + m03 * h) * invdet;
		double r12 = (-m30 * f + m32 * c - m33 * b) * invdet;
		double r13 = ( m20 * f - m22 * c + m23 * b) * invdet;
		double r20 = ( m10 * k - m11 * i + m13 * g) * invdet;
		double r21 = (-m00 * k + m01 * i - m03 * g) * invdet;
		double r22 = ( m30 * e - m31 * c + m33 * a) * invdet;
		double r23 = (-m20 * e + m21 * c - m23 * a) * invdet;
		double r30 = (-m10 * j + m11 * h - m12 * g) * invdet;
		double r31 = ( m00 * j - m01 * h + m02 * g) * invdet;
		double r32 = (-m30 * d + m31 * b - m32 * a) * invdet;
		double r33 = ( m20 * d - m21 * b + m22 * a) * invdet;

		return Mat4(r00, r01, r02, r03, r10, r11, r12, r13, r20, r21, r22, r23, r30, r31, r32, r33);
	}

	Mat4 translate(double x, double y, double z) {
		double r30 = m00 * x +m10 * y + m20 * z + m30;
		double r31 = m01 * x +m11 * y + m21 * z + m31;
		double r32 = m02 * x +m12 * y + m22 * z + m32;
		double r33 = m03 * x +m13 * y + m23 * z + m33;

		return Mat4(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, r30, r31, r32, r33);
	}

	Mat4 rotate(double angle, double x, double y, double z) {
		double s = sin(angle);
		double c = cos(angle);
		double C = (1.0 - c);

		double rm00 = x * x * (1.0 - c) + c;
		double rm01 = x * y * (1.0 - c) + z * s;
		double rm02 = x * z * (1.0 - c) - y * s;
		double rm10 = x * y * (1.0 - c) - z * s;
		double rm11 = y * y * (1.0 - c) + c;
		double rm12 = y * z * (1.0 - c) + x * s;
		double rm20 = x * z * (1.0 - c) + y * s;
		double rm21 = y * z * (1.0 - c) - x * s;
		double rm22 = z * z * (1.0 - c) + c;
		double r00 = m00 * rm00 + m10 * rm01 + m20 * rm02;
		double r01 = m01 * rm00 + m11 * rm01 + m21 * rm02;
		double r02 = m02 * rm00 + m12 * rm01 + m22 * rm02;
		double r03 = m03 * rm00 + m13 * rm01 + m23 * rm02;
		double r10 = m00 * rm10 + m10 * rm11 + m20 * rm12;
		double r11 = m01 * rm10 + m11 * rm11 + m21 * rm12;
		double r12 = m02 * rm10 + m12 * rm11 + m22 * rm12;
		double r13 = m03 * rm10 + m13 * rm11 + m23 * rm12;
		double r20 = m00 * rm20 + m10 * rm21 + m20 * rm22;
		double r21 = m01 * rm20 + m11 * rm21 + m21 * rm22;
		double r22 = m02 * rm20 + m12 * rm21 + m22 * rm22;
		double r23 = m03 * rm20 + m13 * rm21 + m23 * rm22;

		return Mat4(r00, r01, r02, r03, r10, r11, r12, r13, r20, r21, r22, r23, m30, m31, m32, m33);
	}

	Mat4 perspective(float fov, float aspect, float zNear, float zFar) {
		double t = tan(fov / 0.5);
		double t00 = 1.0 / (t * aspect);
		double t11 = 1.0 / t;
		double t22 = (zFar + zNear) / (zNear - zFar);
		double t32 = (zFar + zFar) * zNear / (zNear - zFar);
		double r00 = m00 * t00;
		double r01 = m01 * t00;
		double r02 = m02 * t00;
		double r03 = m03 * t00;
		double r10 = m10 * t11;
		double r11 = m11 * t11;
		double r12 = m12 * t11;
		double r13 = m13 * t11;
		double r30 = m20 * t32;
		double r31 = m21 * t32;
		double r32 = m22 * t32;
		double r33 = m23 * t32;
		double r20 = m20 * t22 - m30;
		double r21 = m21 * t22 - m31;
		double r22 = m22 * t22 - m32;
		double r23 = m23 * t22 - m33;
		return Mat4(r00, r01, r02, r03, r10, r11, r12, r13, r20, r21, r22, r23, r30, r31, r32, r33);
	}

	Mat4 operator+(const Mat4& other) {
		return Mat4(
			m00 + other.m00,
			m01 + other.m01,
			m02 + other.m02,
			m03 + other.m03,
			m10 + other.m10,
			m11 + other.m11,
			m12 + other.m12,
			m13 + other.m13,
			m20 + other.m20,
			m21 + other.m21,
			m22 + other.m22,
			m23 + other.m23,
			m30 + other.m30,
			m31 + other.m31,
			m32 + other.m32,
			m33 + other.m33
		);
	}

	Mat4 operator-(const Mat4& other) {
		return Mat4(
			m00 - other.m00,
			m01 - other.m01,
			m02 - other.m02,
			m03 - other.m03,
			m10 - other.m10,
			m11 - other.m11,
			m12 - other.m12,
			m13 - other.m13,
			m20 - other.m20,
			m21 - other.m21,
			m22 - other.m22,
			m23 - other.m23,
			m30 - other.m30,
			m31 - other.m31,
			m32 - other.m32,
			m33 - other.m33
		);
	}

	Mat4 operator-() {
		return Mat4(-m00, -m01, -m02, -m03, -m10, -m11, -m12, -m13, -m20, -m21, -m22, -m23, -m30, -m31, -m32, -m33);
	}

	Mat4 operator*(const Mat4& other) {
		double r00 = m00 * other.m00 + m10 * other.m01 + m20 * other.m02 + m30 * other.m03;
		double r01 = m01 * other.m00 + m11 * other.m01 + m21 * other.m02 + m31 * other.m03;
		double r02 = m02 * other.m00 + m12 * other.m01 + m22 * other.m02 + m32 * other.m03;
		double r03 = m03 * other.m00 + m13 * other.m01 + m23 * other.m02 + m33 * other.m03;
		double r10 = m00 * other.m10 + m10 * other.m11 + m20 * other.m12 + m30 * other.m13;
		double r11 = m01 * other.m10 + m11 * other.m11 + m21 * other.m12 + m31 * other.m13;
		double r12 = m02 * other.m10 + m12 * other.m11 + m22 * other.m12 + m32 * other.m13;
		double r13 = m03 * other.m10 + m13 * other.m11 + m23 * other.m12 + m33 * other.m13;
		double r20 = m00 * other.m20 + m10 * other.m21 + m20 * other.m22 + m30 * other.m23;
		double r21 = m01 * other.m20 + m11 * other.m21 + m21 * other.m22 + m31 * other.m23;
		double r22 = m02 * other.m20 + m12 * other.m21 + m22 * other.m22 + m32 * other.m23;
		double r23 = m03 * other.m20 + m13 * other.m21 + m23 * other.m22 + m33 * other.m23;
		double r30 = m00 * other.m30 + m10 * other.m31 + m20 * other.m32 + m30 * other.m33;
		double r31 = m01 * other.m30 + m11 * other.m31 + m21 * other.m32 + m31 * other.m33;
		double r32 = m02 * other.m30 + m12 * other.m31 + m22 * other.m32 + m32 * other.m33;
		double r33 = m03 * other.m30 + m13 * other.m31 + m23 * other.m32 + m33 * other.m33;

		return Mat4(r00, r01, r02, r03, r10, r11, r12, r13, r20, r21, r22, r23, r30, r31, r32, r33);
	}

	friend std::ostream& operator<<(std::ostream& os, const Mat4& matrix) {
		os << "mat4(";
		for (int i = 0; i < 15; i++) {
			os << matrix.m[i];
			os << (((i + 1) % 4 == 0)? "; " : ", ");
		}
		os << matrix.m[15] << ")";

		return os;
	}

	std::string str() {
		std::stringstream ss;
		ss << std::string("mat4(");
		for (int i = 0; i < 15; i++) {
			ss << m[i];
			ss << (((i + 1) % 4 == 0) ? "; " : ", ");
		}
		ss << m[15] << ")";

		return ss.str();
	}

};

namespace Mat4Util {
	Mat4 ZERO = Mat4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	Mat4 IDENTITY = Mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
};