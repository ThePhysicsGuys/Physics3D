#pragma once
#include <cmath>
#include <string>
#include <sstream>

template<typename N>
struct Mat4Template {
public:
	union {
		struct {
			N m00;
			N m01;
			N m02;
			N m03;
			N m10;
			N m11;
			N m12;
			N m13;
			N m20;
			N m21;
			N m22;
			N m23;
			N m30;
			N m31;
			N m32;
			N m33;
		};

		N m[16];
	};

	Mat4Template(N m00, N m01, N m02, N m03, N m10, N m11, N m12, N m13, N m20, N m21, N m22, N m23, N m30, N m31, N m32, N m33) :
		m00(m00), m01(m01), m02(m02), m03(m03), m10(m10), m11(m11), m12(m12), m13(m13), m20(m20), m21(m21), m22(m22), m23(m23), m30(m30), m31(m31), m32(m32), m33(m33) {};

	Mat4Template(const N other[16]) {
		for (int i = 0; i < 16; i++) {
			m[i] = other[i];
		}
	};

	Mat4Template(const Mat4Template& other) {
		for (int i = 0; i < 16; i++) {
			m[i] = other.m[i];
		}
	}

	N det() {
		N a = m00 * m11 - m01 * m10;
		N b = m00 * m12 - m02 * m10;
		N c = m00 * m13 - m03 * m10;
		N d = m01 * m12 - m02 * m11;
		N e = m01 * m13 - m03 * m11;
		N f = m02 * m13 - m03 * m12;
		N g = m20 * m31 - m21 * m30;
		N h = m20 * m32 - m22 * m30;
		N i = m20 * m33 - m23 * m30;
		N j = m21 * m32 - m22 * m31;
		N k = m21 * m33 - m23 * m31;
		N l = m22 * m33 - m23 * m32;
		N det = a * l - b * k + c * j + d * i - e * h + f * g;

		return det;
	}

	Mat4Template inverse() {
		N a = m00 * m11 - m01 * m10;
		N b = m00 * m12 - m02 * m10;
		N c = m00 * m13 - m03 * m10;
		N d = m01 * m12 - m02 * m11;
		N e = m01 * m13 - m03 * m11;
		N f = m02 * m13 - m03 * m12;
		N g = m20 * m31 - m21 * m30;
		N h = m20 * m32 - m22 * m30;
		N i = m20 * m33 - m23 * m30;
		N j = m21 * m32 - m22 * m31;
		N k = m21 * m33 - m23 * m31;
		N l = m22 * m33 - m23 * m32;
		N normdet = a * l - b * k + c * j + d * i - e * h + f * g;

		N invdet = 1.0 / normdet;

		N r00 = ( m11 * l - m12 * k + m13 * j) * invdet;
		N r01 = (-m01 * l + m02 * k - m03 * j) * invdet;
		N r02 = ( m31 * f - m32 * e + m33 * d) * invdet;
		N r03 = (-m21 * f + m22 * e - m23 * d) * invdet;
		N r10 = (-m10 * l + m12 * i - m13 * h) * invdet;
		N r11 = ( m00 * l - m02 * i + m03 * h) * invdet;
		N r12 = (-m30 * f + m32 * c - m33 * b) * invdet;
		N r13 = ( m20 * f - m22 * c + m23 * b) * invdet;
		N r20 = ( m10 * k - m11 * i + m13 * g) * invdet;
		N r21 = (-m00 * k + m01 * i - m03 * g) * invdet;
		N r22 = ( m30 * e - m31 * c + m33 * a) * invdet;
		N r23 = (-m20 * e + m21 * c - m23 * a) * invdet;
		N r30 = (-m10 * j + m11 * h - m12 * g) * invdet;
		N r31 = ( m00 * j - m01 * h + m02 * g) * invdet;
		N r32 = (-m30 * d + m31 * b - m32 * a) * invdet;
		N r33 = ( m20 * d - m21 * b + m22 * a) * invdet;

		return Mat4Template(r00, r01, r02, r03, r10, r11, r12, r13, r20, r21, r22, r23, r30, r31, r32, r33);
	}

	Mat4Template translate(N x, N y, N z) {
		N r30 = m00 * x +m10 * y + m20 * z + m30;
		N r31 = m01 * x +m11 * y + m21 * z + m31;
		N r32 = m02 * x +m12 * y + m22 * z + m32;
		N r33 = m03 * x +m13 * y + m23 * z + m33;

		return Mat4Template(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, r30, r31, r32, r33);
	}

	Mat4Template rotate(N angle, N x, N y, N z) {
		N s = sin(angle);
		N c = cos(angle);
		N C = (1.0 - c);

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
		N r03 = m03 * rm00 + m13 * rm01 + m23 * rm02;
		N r10 = m00 * rm10 + m10 * rm11 + m20 * rm12;
		N r11 = m01 * rm10 + m11 * rm11 + m21 * rm12;
		N r12 = m02 * rm10 + m12 * rm11 + m22 * rm12;
		N r13 = m03 * rm10 + m13 * rm11 + m23 * rm12;
		N r20 = m00 * rm20 + m10 * rm21 + m20 * rm22;
		N r21 = m01 * rm20 + m11 * rm21 + m21 * rm22;
		N r22 = m02 * rm20 + m12 * rm21 + m22 * rm22;
		N r23 = m03 * rm20 + m13 * rm21 + m23 * rm22;

		return Mat4Template(r00, r01, r02, r03, r10, r11, r12, r13, r20, r21, r22, r23, m30, m31, m32, m33);
	}

	Mat4Template perspective(float fov, float aspect, float zNear, float zFar) {
		N t = tan(fov / 2);
		N t00 = 1 / (t * aspect);
		N t11 = 1 / t;
		N t22 = (zFar + zNear) / (zNear - zFar);
		N t32 = (zFar + zFar) * zNear / (zNear - zFar);
		N r00 = m00 * t00;
		N r01 = m01 * t00;
		N r02 = m02 * t00;
		N r03 = m03 * t00;
		N r10 = m10 * t11;
		N r11 = m11 * t11;
		N r12 = m12 * t11;
		N r13 = m13 * t11;
		N r30 = m20 * t32;
		N r31 = m21 * t32;
		N r32 = m22 * t32;
		N r33 = m23 * t32;
		N r20 = m20 * t22 - m30;
		N r21 = m21 * t22 - m31;
		N r22 = m22 * t22 - m32;
		N r23 = m23 * t22 - m33;
		return Mat4Template(r00, r01, r02, r03, r10, r11, r12, r13, r20, r21, r22, r23, r30, r31, r32, r33);
	}

	Mat4Template operator+(const Mat4Template& other) const {
		return Mat4Template(
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

	Mat4Template operator-(const Mat4Template& other) const {
		return Mat4Template(
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

	Mat4Template operator-() const {
		return Mat4Template(-m00, -m01, -m02, -m03, -m10, -m11, -m12, -m13, -m20, -m21, -m22, -m23, -m30, -m31, -m32, -m33);
	}

	Mat4Template operator~() const {
		return inverse();
	}

	Mat4Template operator*(const Mat4Template& other) const {
		N r00 = m00 * other.m00 + m10 * other.m01 + m20 * other.m02 + m30 * other.m03;
		N r01 = m01 * other.m00 + m11 * other.m01 + m21 * other.m02 + m31 * other.m03;
		N r02 = m02 * other.m00 + m12 * other.m01 + m22 * other.m02 + m32 * other.m03;
		N r03 = m03 * other.m00 + m13 * other.m01 + m23 * other.m02 + m33 * other.m03;
		N r10 = m00 * other.m10 + m10 * other.m11 + m20 * other.m12 + m30 * other.m13;
		N r11 = m01 * other.m10 + m11 * other.m11 + m21 * other.m12 + m31 * other.m13;
		N r12 = m02 * other.m10 + m12 * other.m11 + m22 * other.m12 + m32 * other.m13;
		N r13 = m03 * other.m10 + m13 * other.m11 + m23 * other.m12 + m33 * other.m13;
		N r20 = m00 * other.m20 + m10 * other.m21 + m20 * other.m22 + m30 * other.m23;
		N r21 = m01 * other.m20 + m11 * other.m21 + m21 * other.m22 + m31 * other.m23;
		N r22 = m02 * other.m20 + m12 * other.m21 + m22 * other.m22 + m32 * other.m23;
		N r23 = m03 * other.m20 + m13 * other.m21 + m23 * other.m22 + m33 * other.m23;
		N r30 = m00 * other.m30 + m10 * other.m31 + m20 * other.m32 + m30 * other.m33;
		N r31 = m01 * other.m30 + m11 * other.m31 + m21 * other.m32 + m31 * other.m33;
		N r32 = m02 * other.m30 + m12 * other.m31 + m22 * other.m32 + m32 * other.m33;
		N r33 = m03 * other.m30 + m13 * other.m31 + m23 * other.m32 + m33 * other.m33;

		return Mat4Template(r00, r01, r02, r03, r10, r11, r12, r13, r20, r21, r22, r23, r30, r31, r32, r33);
	}

	friend std::ostream& operator<<(std::ostream& os, const Mat4Template& matrix) const {
		os << "Mat4Template(";
		for (int i = 0; i < 15; i++) {
			os << matrix.m[i];
			os << (((i + 1) % 4 == 0)? "; " : ", ");
		}
		os << matrix.m[15] << ")";

		return os;
	}

	std::string str() {
		std::stringstream ss;
		ss << std::string("Mat4Template(");
		for (int i = 0; i < 15; i++) {
			ss << m[i];
			ss << (((i + 1) % 4 == 0) ? "; " : ", ");
		}
		ss << m[15] << ")";

		return ss.str();
	}

};

typedef Mat4Template<double>	Mat4;
typedef Mat4Template<float>		Mat4f;
typedef Mat4Template<long long>	Mat4l;

namespace Mat4TemplateUtil {
	Mat4 ZERO = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	Mat4 IDENTITY = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
};