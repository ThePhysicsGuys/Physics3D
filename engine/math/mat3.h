#pragma once

template<typename N>
struct Mat3Template;
template<typename N>
struct SymmetricMat3Template;
template<typename N>
struct DiagonalMat3Template;

#include "vec3.h"
#include "../templateUtils.h"

template<typename N>
struct EigenValues {
	N values[3];

	EigenValues(N a, N b, N c) : values{a, b, c} {}

	Mat3Template<N> asDiagonalMatrix() const {
		return Mat3Template<N>(values[0], 0, 0,
							   0, values[1], 0,
							   0, 0, values[2]);
	}

	const N& operator[](int index) const { return values[index]; }
	N& operator[](int index) { return values[index]; }
};

template<typename N>
struct EigenSet {
	EigenValues<N> eigenValues;
	Mat3Template<N> eigenVectors;
	EigenSet(EigenValues<N> eigenValues, Mat3Template<N> eigenVectors) : eigenValues(eigenValues), eigenVectors(eigenVectors) {};
};

struct M33Type {};
struct SymmetricM33Type : M33Type {};

template<typename N>
struct Mat3Template : public M33Type {
public:
	union {
		struct {
			N m00, m01, m02;
			N m10, m11, m12;
			N m20, m21, m22;
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

	Mat3Template() : Mat3Template(1,0,0,0,1,0,0,0,1) {};

	template<typename OtherN>
	Mat3Template(const Mat3Template<OtherN>& o) : Mat3Template(static_cast<N>(o.m00), static_cast<N>(o.m01), static_cast<N>(o.m02), 
															   static_cast<N>(o.m10), static_cast<N>(o.m11), static_cast<N>(o.m12), 
															   static_cast<N>(o.m20), static_cast<N>(o.m21), static_cast<N>(o.m22)) {}

	template<typename OtherN>
	Mat3Template(const SymmetricMat3Template<OtherN>& o) : Mat3Template(static_cast<N>(o.m00), static_cast<N>(o.m01), static_cast<N>(o.m02), 
																		static_cast<N>(o.m10), static_cast<N>(o.m11), static_cast<N>(o.m12), 
																		static_cast<N>(o.m20), static_cast<N>(o.m21), static_cast<N>(o.m22)) {}

	template<typename OtherN>
	Mat3Template(const DiagonalMat3Template<OtherN>& o) : Mat3Template(static_cast<N>(o.m00), static_cast<N>(o.m01), static_cast<N>(o.m02), 
																	   static_cast<N>(o.m10), static_cast<N>(o.m11), static_cast<N>(o.m12), 
																	   static_cast<N>(o.m20), static_cast<N>(o.m21), static_cast<N>(o.m22)) {}

	N det() const {
		return m00 * (m11 * m22 - m12 * m21) - m01 * (m10 * m22 - m12 * m20) + m02 * (m10 * m21 - m11 * m20);
	}

	Mat3Template<N> inverse() const {
		N r00 = m11 * m22 - m21 * m12;
		N r01 = m21 * m02 - m01 * m22;
		N r02 = m01 * m12 - m11 * m02;
		N r10 = m20 * m12 - m10 * m22;
		N r11 = m00 * m22 - m20 * m02;
		N r12 = m10 * m02 - m00 * m12;
		N r20 = m10 * m21 - m20 * m11;
		N r21 = m20 * m01 - m00 * m21;
		N r22 = m00 * m11 - m10 * m01;
		
		return Mat3Template(r00, r01, r02, r10, r11, r12, r20, r21, r22) * (1/det());
	}

	Mat3Template<N> rotate(N angle, N x, N y, N z) const;

	Mat3Template<N> transpose() const {
		return Mat3Template<N>(m00, m10, m20, m01, m11, m21, m02, m12, m22);
	}

	Mat3Template<N> operator~() const {
		return inverse();
	}

	template<typename N>
	Mat3Template<N> operator-() const {
		return Mat3Template<N>(-m00, -m01, -m02, -m10, -m11, -m12, -m20, -m21, -m22);
	}

	Mat3Template<N>& operator+=(const Mat3Template<N>& other) {
		m00 += other.m00; m01 += other.m01; m02 += other.m02;
		m10 += other.m10; m11 += other.m11; m12 += other.m12;
		m20 += other.m20; m21 += other.m21; m22 += other.m22;
		return *this;
	}

	Mat3Template<N>& operator-=(const Mat3Template<N>& other) {
		m00 -= other.m00; m01 -= other.m01; m02 -= other.m02;
		m10 -= other.m10; m11 -= other.m11; m12 -= other.m12;
		m20 -= other.m20; m21 -= other.m21; m22 -= other.m22;
		return *this;
	}

	Mat3Template operator*(const N& f) const {
		return Mat3Template<N>(
			m00 * f, m01 * f, m02 * f,
			m10 * f, m11 * f, m12 * f,
			m20 * f, m21 * f, m22 * f
		);
	}
	
	Mat3Template<N>& operator*=(const N& f) {
		m00 *= f; m01 *= f; m02 *= f;
		m10 *= f; m11 *= f; m12 *= f;
		m20 *= f; m21 *= f; m22 *= f;
		return *this;
	}

	// EigenSet<N> getEigenDecomposition() const;
};


template<typename N>
struct SymmetricMat3Template : public SymmetricM33Type {
public:
	union {
		struct {
			N m00, m11, m22;
			union { N m10; N m01; N s01; };
			union { N m20; N m02; N s02; };
			union { N m21; N m12; N s12; };
		};

		N m[6];
	};

	SymmetricMat3Template() : SymmetricMat3Template(1,1,1,0,0,0) {};
	SymmetricMat3Template(N m00, N m11, N m22, N s01, N s02, N s12) :
		m00(m00), m11(m11), m22(m22), s01(s01), s02(s02), s12(s12) {};

	template<template<typename> typename SMat, typename OtherN, IS_SUBCLASS_OF(SMat<N>, SymmetricM33Type)>
	SymmetricMat3Template(const SMat<OtherN>& o) : SymmetricMat3Template(o.m00, o.m11, o.m22, o.m01, o.m02, o.m12) {}


	N det() const {
		return m00 * (m11 * m22 - s12 * s12) - s01 * (s01 * m22 - s12 * s02) + s02 * (s01 * s12 - m11 * s02);
	}

	SymmetricMat3Template<N> inverse() const {
		N r00 = m11 * m22 - s12 * s12;
		N r11 = m00 * m22 - s02 * s02;
		N r22 = m00 * m11 - s01 * s01;
		N r01 = s12 * s02 - s01 * m22;
		N r02 = s01 * s12 - m11 * s02;
		N r12 = s01 * s02 - m00 * s12;

		return SymmetricMat3Template(r00, r11, r22, r01, r02, r12) * (1 / det());
	}

	SymmetricMat3Template<N> transpose() const {
		return *this;
	}

	SymmetricMat3Template<N> operator~() const {
		return inverse();
	}

	template<typename N>
	SymmetricMat3Template<N> operator-() const {
		return SymmetricMat3Template<N>(-m00, -m11, -m22, -s01, -s02, -s12);
	}

	SymmetricMat3Template<N>& operator+=(const SymmetricMat3Template<N>& other) {
		m00 += other.m00; m11 += other.m11; m22 += other.m22;
		s01 += other.s01; s02 += other.s02; s12 += other.s12;
		return *this;
	}

	SymmetricMat3Template<N>& operator-=(const SymmetricMat3Template<N>& other) {
		m00 -= other.m00; m11 -= other.m11; m22 -= other.m22;
		s01 -= other.s01; s02 -= other.s02; s12 -= other.s12;
		return *this;
	}

	SymmetricMat3Template operator*(const N& f) const {
		return SymmetricMat3Template<N>(
			m00*f, m11*f, m22*f,
			s01*f, s02*f, s12*f
		);
	}

	SymmetricMat3Template<N>& operator*=(const N& f) {
		m00 *= f; m11 *= f; m22 *= f;
		s01 *= f; s02 *= f; s12 *= f;
		return *this;
	}

	EigenSet<N> getEigenDecomposition() const;
};

template<typename N>
struct DiagonalMat3Template : public SymmetricM33Type {
public:
	static const N m01;
	static const N m10;
	static const N m02;
	static const N m20;
	static const N m12;
	static const N m21;
	static const N s01;
	static const N s02;
	static const N s12;
	union {
		struct {
			N m00, m11, m22;
		};

		N m[3];
	};

	DiagonalMat3Template(N m00, N m11, N m22) :
		m00(m00), m11(m11), m22(m22) {
	};

	DiagonalMat3Template(const N other[3]) {
		for(int i = 0; i < 3; i++) {
			m[i] = other[i];
		}
	};

	DiagonalMat3Template() : m00(1), m11(1), m22(1) {};

	DiagonalMat3Template(const DiagonalMat3Template& other) {
		for(int i = 0; i < 3; i++) {
			m[i] = other.m[i];
		}
	}

	N det() const {
		return m00 * m11 * m22;
	}
	DiagonalMat3Template<N> inverse() const {
		return DiagonalMat3Template<N>(1 / m00, 1 / m11, 1 / m22);
	}

	DiagonalMat3Template<N> transpose() const {
		return *this;
	}
	DiagonalMat3Template<N> operator~() const {
		return inverse();
	}
	template<typename N>
	DiagonalMat3Template<N> operator-() const {
		return DiagonalMat3Template<N>(-m00, -m11, -m22);
	}

	DiagonalMat3Template<N>& operator+=(const DiagonalMat3Template<N>& other) {
		m00 += other.m00; m11 += other.m11; m22 += other.m22;
		return *this;
	}

	DiagonalMat3Template<N>& operator-=(const DiagonalMat3Template<N>& other) {
		m00 -= other.m00; m11 -= other.m11; m22 -= other.m22;
		return *this;
	}

	DiagonalMat3Template operator*(const N& f) const {
		return DiagonalMat3Template<N>(
			m00*f, m11*f, m22*f
		);
	}

	DiagonalMat3Template<N>& operator*=(const N& f) {
		m00 *= f; m11 *= f; m22 *= f;
		return *this;
	}

	EigenSet<N> getEigenDecomposition() const;
};
template<typename N> const N DiagonalMat3Template<N>::m01 = 0;
template<typename N> const N DiagonalMat3Template<N>::m10 = 0;
template<typename N> const N DiagonalMat3Template<N>::m02 = 0;
template<typename N> const N DiagonalMat3Template<N>::m20 = 0;
template<typename N> const N DiagonalMat3Template<N>::m12 = 0;
template<typename N> const N DiagonalMat3Template<N>::m21 = 0;
template<typename N> const N DiagonalMat3Template<N>::s01 = 0;
template<typename N> const N DiagonalMat3Template<N>::s02 = 0;
template<typename N> const N DiagonalMat3Template<N>::s12 = 0;

template<template<typename> typename Mat, template<typename> typename MatMat, typename N, IS_SUBCLASS_OF(Mat<N>, M33Type), IS_SUBCLASS_OF(MatMat<N>, M33Type)>
Mat3Template<N> operator+(const Mat<N>& self, const MatMat<N>& other) {
	return Mat3Template<N>(
		self.m00 + other.m00, self.m01 + other.m01, self.m02 + other.m02,
		self.m10 + other.m10, self.m11 + other.m11, self.m12 + other.m12,
		self.m20 + other.m20, self.m21 + other.m21, self.m22 + other.m22
	);
}
template<typename N>
SymmetricMat3Template<N> operator+(const SymmetricMat3Template<N>& self, const SymmetricMat3Template<N>& other) {
	return SymmetricMat3Template<N>(
		self.m00 + other.m00, self.m11 + other.m11, self.m22 + other.m22,
		self.m01 + other.m01, self.m02 + other.m02, self.m12 + other.m12
		);
}
template<typename N>
SymmetricMat3Template<N> operator+(const SymmetricMat3Template<N>& self, const DiagonalMat3Template<N>& other) {
	return SymmetricMat3Template<N>(
		self.m00 + other.m00, self.m11 + other.m11, self.m22 + other.m22,
		self.m01 + other.m01, self.m02 + other.m02, self.m12 + other.m12
		);
}
template<typename N>
SymmetricMat3Template<N> operator+(const DiagonalMat3Template<N>& self, const SymmetricMat3Template<N>& other) {
	return SymmetricMat3Template<N>(
		self.m00 + other.m00, self.m11 + other.m11, self.m22 + other.m22,
		self.m01 + other.m01, self.m02 + other.m02, self.m12 + other.m12
		);
}
template<typename N>
DiagonalMat3Template<N> operator+(const DiagonalMat3Template<N>& self, const DiagonalMat3Template<N>& other) {
	return DiagonalMat3Template<N>(
		self.m00 + other.m00, self.m11 + other.m11, self.m22 + other.m22
		);
}


template<template<typename> typename Mat, template<typename> typename MatMat, typename N, IS_SUBCLASS_OF(Mat<N>, M33Type), IS_SUBCLASS_OF(MatMat<N>, M33Type)>
Mat3Template<N> operator-(const Mat<N>& self, const MatMat<N>& other) {
	return Mat3Template<N>(
		self.m00 - other.m00, self.m01 - other.m01, self.m02 - other.m02,
		self.m10 - other.m10, self.m11 - other.m11, self.m12 - other.m12,
		self.m20 - other.m20, self.m21 - other.m21, self.m22 - other.m22
		);
}
template<typename N>
SymmetricMat3Template<N> operator-(const SymmetricMat3Template<N>& self, const SymmetricMat3Template<N>& other) {
	return SymmetricMat3Template<N>(
		self.m00 - other.m00, self.m11 - other.m11, self.m22 - other.m22,
		self.m01 - other.m01, self.m02 - other.m02, self.m12 - other.m12
		);
}
template<typename N>
SymmetricMat3Template<N> operator-(const SymmetricMat3Template<N>& self, const DiagonalMat3Template<N>& other) {
	return SymmetricMat3Template<N>(
		self.m00 - other.m00, self.m11 - other.m11, self.m22 - other.m22,
		self.m01 - other.m01, self.m02 - other.m02, self.m12 - other.m12
		);
}
template<typename N>
SymmetricMat3Template<N> operator-(const DiagonalMat3Template<N>& self, const SymmetricMat3Template<N>& other) {
	return SymmetricMat3Template<N>(
		self.m00 - other.m00, self.m11 - other.m11, self.m22 - other.m22,
		self.m01 - other.m01, self.m02 - other.m02, self.m12 - other.m12
		);
}
template<typename N>
DiagonalMat3Template<N> operator-(const DiagonalMat3Template<N>& self, const DiagonalMat3Template<N>& other) {
	return DiagonalMat3Template<N>(
		self.m00 - other.m00, self.m11 - other.m11, self.m22 - other.m22
	);
}


template<template<typename> typename Mat, template<typename> typename MatMat, typename N, IS_SUBCLASS_OF(Mat<N>, M33Type), IS_SUBCLASS_OF(MatMat<N>, M33Type)>
Mat3Template<N> operator*(const Mat<N>& self, const MatMat<N>& other) {
	N r00 = self.m00 * other.m00 + self.m10 * other.m01 + self.m20 * other.m02;
	N r01 = self.m01 * other.m00 + self.m11 * other.m01 + self.m21 * other.m02;
	N r02 = self.m02 * other.m00 + self.m12 * other.m01 + self.m22 * other.m02;
	N r10 = self.m00 * other.m10 + self.m10 * other.m11 + self.m20 * other.m12;
	N r11 = self.m01 * other.m10 + self.m11 * other.m11 + self.m21 * other.m12;
	N r12 = self.m02 * other.m10 + self.m12 * other.m11 + self.m22 * other.m12;
	N r20 = self.m00 * other.m20 + self.m10 * other.m21 + self.m20 * other.m22;
	N r21 = self.m01 * other.m20 + self.m11 * other.m21 + self.m21 * other.m22;
	N r22 = self.m02 * other.m20 + self.m12 * other.m21 + self.m22 * other.m22;

	return Mat3Template<N>(r00, r01, r02, r10, r11, r12, r20, r21, r22);
}

template<typename N>
SymmetricMat3Template<N> operator*(const SymmetricMat3Template<N>& self, const SymmetricMat3Template<N>& other) {
	N r00 = self.m00 * other.m00 + self.m10 * other.m01 + self.m20 * other.m02;
	N r11 = self.m01 * other.m10 + self.m11 * other.m11 + self.m21 * other.m12;
	N r22 = self.m02 * other.m20 + self.m12 * other.m21 + self.m22 * other.m22;
	N r01 = self.m01 * other.m00 + self.m11 * other.m01 + self.m21 * other.m02;
	N r02 = self.m02 * other.m00 + self.m12 * other.m01 + self.m22 * other.m02;
	N r12 = self.m02 * other.m10 + self.m12 * other.m11 + self.m22 * other.m12;

	return SymmetricMat3Template<N>(r00, r11, r22, r01, r02, r12);
}

template<typename N>
SymmetricMat3Template<N> operator*(const SymmetricMat3Template<N>& self, const DiagonalMat3Template<N>& other) {
	N r00 = self.m00 * other.m00 + self.m10 * other.m01 + self.m20 * other.m02;
	N r11 = self.m01 * other.m10 + self.m11 * other.m11 + self.m21 * other.m12;
	N r22 = self.m02 * other.m20 + self.m12 * other.m21 + self.m22 * other.m22;
	N r01 = self.m01 * other.m00 + self.m11 * other.m01 + self.m21 * other.m02;
	N r02 = self.m02 * other.m00 + self.m12 * other.m01 + self.m22 * other.m02;
	N r12 = self.m02 * other.m10 + self.m12 * other.m11 + self.m22 * other.m12;

	return SymmetricMat3Template<N>(r00, r11, r22, r01, r02, r12);
}

template<typename N>
SymmetricMat3Template<N> operator*(const DiagonalMat3Template<N>& self, const SymmetricMat3Template<N>& other) {
	N r00 = self.m00 * other.m00 + self.m10 * other.m01 + self.m20 * other.m02;
	N r11 = self.m01 * other.m10 + self.m11 * other.m11 + self.m21 * other.m12;
	N r22 = self.m02 * other.m20 + self.m12 * other.m21 + self.m22 * other.m22;
	N r01 = self.m01 * other.m00 + self.m11 * other.m01 + self.m21 * other.m02;
	N r02 = self.m02 * other.m00 + self.m12 * other.m01 + self.m22 * other.m02;
	N r12 = self.m02 * other.m10 + self.m12 * other.m11 + self.m22 * other.m12;

	return SymmetricMat3Template<N>(r00, r11, r22, r01, r02, r12);
}

template<typename N>
DiagonalMat3Template<N> operator*(const DiagonalMat3Template<N>& self, const DiagonalMat3Template<N>& other) {
	N r00 = self.m00 * other.m00;
	N r11 = self.m11 * other.m11;
	N r22 = self.m22 * other.m22;

	return DiagonalMat3Template<N>(r00, r11, r22);
}

template<template<typename> typename Mat, typename N, IS_SUBCLASS_OF(Mat<N>, M33Type)>
Vec3Template<N> operator*(const Mat<N>& mat, const Vec3Template<N>& vec) {
	return Vec3Template<N>(
		mat.m00 * vec.x + mat.m10 * vec.y + mat.m20 * vec.z,
		mat.m01 * vec.x + mat.m11 * vec.y + mat.m21 * vec.z,
		mat.m02 * vec.x + mat.m12 * vec.y + mat.m22 * vec.z
		);
}

template<typename N>
Mat3Template<N> outer(const Vec3Template<N>& self, const Vec3Template<N>& other) {
	return Mat3Template<N>(self.x*other.x, self.x*other.y, self.x*other.z,
						   self.y*other.x, self.y*other.y, self.y*other.z,
						   self.z*other.x, self.z*other.y, self.z*other.z);
}

typedef Mat3Template<double>	Mat3;
typedef Mat3Template<float>		Mat3f;
typedef Mat3Template<long long>	Mat3l;

typedef SymmetricMat3Template<double>		SymmetricMat3;
typedef SymmetricMat3Template<float>		SymmetricMat3f;
typedef SymmetricMat3Template<long long>	SymmetricMat3l;

typedef DiagonalMat3Template<double>		DiagonalMat3;
typedef DiagonalMat3Template<float>			DiagonalMat3f;
typedef DiagonalMat3Template<long long>		DiagonalMat3l;

typedef Mat3Template<double>	RotMat3;
typedef Mat3Template<float>		RotMat3f;
typedef Mat3Template<long long>	RotMat3l;


template<typename N>
Mat3Template<N> rotX(N angle);

template<typename N>
Mat3Template<N> rotY(N angle);

template<typename N>
Mat3Template<N> rotZ(N angle);

template<typename N>
Mat3Template<N> fromEulerAngles(N alpha, N beta, N gamma);

template<typename N> 
Mat3Template<N> fromRotationVec(Vec3Template<N> rotVec);
