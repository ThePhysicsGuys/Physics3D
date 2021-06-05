#pragma once

#include "../physics/math/linalg/vec.h"
#include "../physics/math/linalg/largeMatrix.h"
#include "../physics/math/linalg/mat.h"
#include "../physics/math/linalg/eigen.h"
#include "../physics/math/cframe.h"
#include "../physics/math/position.h"
#include "../physics/math/globalCFrame.h"
#include "../physics/math/taylorExpansion.h"
#include "../physics/math/boundingBox.h"

#include "../physics/motion.h"
#include "../physics/relativeMotion.h"

#include <utility>
#include <array>
#include <vector>
#include <type_traits>

#define IS_SUBCLASS_OF(Child, BaseClass) typename std::enable_if<std::is_base_of<BaseClass, Child>::value>::type* = nullptr
#define IS_ARITHMETIC(Type) typename std::enable_if<std::is_arithmetic<Type>::value>::type* = nullptr

template<typename Num1, typename Num2, typename Tol, IS_ARITHMETIC(Num1), IS_ARITHMETIC(Num2)>
bool tolerantEquals(const Num1& first, const Num2& second, Tol tolerance) {
	auto diff = first - second;
	return diff <= tolerance && -diff <= tolerance;
}

template<typename Num1, typename Num2, typename Tol, IS_ARITHMETIC(Num1), IS_ARITHMETIC(Num2)>
bool tolerantNotEquals(const Num1& first, const Num2& second, Tol tolerance) {
	auto diff = first - second;
	return diff > tolerance && -diff > tolerance;
}

template<typename Num1, typename Num2, typename Tol, IS_ARITHMETIC(Num1), IS_ARITHMETIC(Num2)>
bool tolerantLessThan(const Num1& first, const Num2& second, Tol tolerance) {
	return first < second + tolerance;
}

template<typename Num1, typename Num2, typename Tol, IS_ARITHMETIC(Num1), IS_ARITHMETIC(Num2)>
bool tolerantGreaterThan(const Num1& first, const Num2& second, Tol tolerance) {
	return first + tolerance > second;
}

template<typename Num1, typename Num2, typename Tol, IS_ARITHMETIC(Num1), IS_ARITHMETIC(Num2)>
bool tolerantLessOrEqual(const Num1& first, const Num2& second, Tol tolerance) {
	return first <= second + tolerance;
}

template<typename Num1, typename Num2, typename Tol, IS_ARITHMETIC(Num1), IS_ARITHMETIC(Num2)>
bool tolerantGreaterOrEqual(const Num1& first, const Num2& second, Tol tolerance) {
	return first + tolerance >= second;
}

template<typename Num1, typename Num2, typename Tol, size_t Size>
bool tolerantEquals(const P3D::Vector<Num1, Size>& first, const P3D::Vector<Num2, Size>& second, Tol tolerance) {
	for(size_t i = 0; i < Size; i++) {
		if(!tolerantEquals(first[i], second[i], tolerance)) return false;
	}
	return true;
}
template<typename Num1, typename Num2, typename Tol>
bool tolerantEquals(const P3D::LargeVector<Num1>& first, const P3D::LargeVector<Num2>& second, Tol tolerance) {
	if(first.n != second.n) throw "Dimensions must match!";
	for(size_t i = 0; i < first.n; i++) {
		if(!tolerantEquals(first[i], second[i], tolerance)) return false;
	}
	return true;
}
template<typename Num1, typename Num2, typename Tol, size_t Width, size_t Height>
bool tolerantEquals(const P3D::Matrix<Num1, Height, Width>& first, const P3D::Matrix<Num2, Height, Width>& second, Tol tolerance) {
	for(size_t row = 0; row < Height; row++)
		for(size_t col = 0; col < Width; col++)
			if(!tolerantEquals(first(row, col), second(row, col), tolerance))
				return false;

	return true;
}

template<typename Num1, typename Num2, typename Tol, size_t Size>
bool tolerantEquals(const P3D::SymmetricMatrix<Num1, Size>& first, const P3D::SymmetricMatrix<Num2, Size>& second, Tol tolerance) {
	for(size_t row = 0; row < Size; row++)
		for(size_t col = 0; col < Size; col++)
			if(!tolerantEquals(first(row, col), second(row, col), tolerance))
				return false;

	return true;
}

template<typename Num1, typename Num2, typename Tol, size_t Size>
bool tolerantEquals(const P3D::DiagonalMatrix<Num1, Size>& first, const P3D::DiagonalMatrix<Num2, Size>& second, Tol tolerance) {
	for(size_t i = 0; i < Size; i++)
		if(!tolerantEquals(first[i], second[i], tolerance))
			return false;

	return true;
}

template<typename Num1, typename Num2, typename Tol>
bool tolerantEquals(const P3D::LargeMatrix<Num1>& first, const P3D::LargeMatrix<Num2>& second, Tol tolerance) {
	if(first.w != second.w || first.h != second.h) throw "Dimensions must match!";
	for(size_t row = 0; row < first.h; row++)
		for(size_t col = 0; col < first.w; col++)
			if(!tolerantEquals(first.get(row, col), second.get(row, col), tolerance))
				return false;

	return true;
}

template<typename Num1, typename Num2, typename Tol>
bool tolerantEquals(const P3D::LargeSymmetricMatrix<Num1>& first, const P3D::LargeSymmetricMatrix<Num2>& second, Tol tolerance) {
	if(first.size != second.size) throw "Dimensions must match!";
	for(size_t row = 0; row < first.size; row++)
		for(size_t col = row; col < first.size; col++)
			if(!tolerantEquals(first.get(row, col), second.get(row, col), tolerance))
				return false;

	return true;
}

template<typename Tol>
bool tolerantEquals(const P3D::Position& a, const P3D::Position& b, Tol tolerance) {
	P3D::Vec3 delta = a - b;
	return tolerantEquals(delta, P3D::Vec3(0, 0, 0), tolerance);
}

template<typename T, typename Tol>
bool tolerantEquals(const P3D::Quaternion<T>& a, const P3D::Quaternion<T>& b, Tol tolerance) {
	return
		tolerantEquals(a.w, b.w, tolerance) &&
		tolerantEquals(a.i, b.i, tolerance) &&
		tolerantEquals(a.j, b.j, tolerance) &&
		tolerantEquals(a.k, b.k, tolerance);
}

template<typename T, typename Tol>
bool tolerantEquals(const P3D::MatrixRotationTemplate<T>& a, const P3D::MatrixRotationTemplate<T>& b, Tol tolerance) {
	return tolerantEquals(a.asRotationMatrix(), b.asRotationMatrix(), tolerance);
}

template<typename T, typename Tol>
bool tolerantEquals(const P3D::QuaternionRotationTemplate<T>& a, const P3D::QuaternionRotationTemplate<T>& b, Tol tolerance) {
	P3D::Quaternion<T> aq = a.asRotationQuaternion();
	P3D::Quaternion<T> bq = b.asRotationQuaternion();
	// Quaternions double cover the plane of possible rotations, -q and q express the same rotation. 
	// Therefore we must make sure we are comparing correctly
	if(dot(aq, bq) > 0) { // quaternions are aligned
		return tolerantEquals(aq, bq, tolerance);
	} else { // quaternions are not aligned
		return tolerantEquals(aq, -bq, tolerance);
	}
}

template<typename Tol>
bool tolerantEquals(const P3D::CFrame& first, const P3D::CFrame& second, Tol tolerance) {
	return tolerantEquals(first.position, second.position, tolerance) &&
		tolerantEquals(first.rotation, second.rotation, tolerance);
}

template<typename Tol>
bool tolerantEquals(const P3D::GlobalCFrame& first, const P3D::GlobalCFrame& second, Tol tolerance) {
	return tolerantEquals(first.position, second.position, tolerance) &&
		tolerantEquals(first.rotation, second.rotation, tolerance);
}

template<typename Tol, typename N>
bool tolerantEquals(const P3D::EigenValues<N, 3>& a, const P3D::EigenValues<N, 3>& b, Tol tolerance) {
	return tolerantEquals(a[0], b[0], tolerance) && tolerantEquals(a[1], b[1], tolerance) && tolerantEquals(a[2], b[2], tolerance) ||
		tolerantEquals(a[0], b[0], tolerance) && tolerantEquals(a[1], b[2], tolerance) && tolerantEquals(a[2], b[1], tolerance) ||
		tolerantEquals(a[0], b[1], tolerance) && tolerantEquals(a[1], b[0], tolerance) && tolerantEquals(a[2], b[2], tolerance) ||
		tolerantEquals(a[0], b[1], tolerance) && tolerantEquals(a[1], b[2], tolerance) && tolerantEquals(a[2], b[0], tolerance) ||
		tolerantEquals(a[0], b[2], tolerance) && tolerantEquals(a[1], b[0], tolerance) && tolerantEquals(a[2], b[1], tolerance) ||
		tolerantEquals(a[0], b[2], tolerance) && tolerantEquals(a[1], b[1], tolerance) && tolerantEquals(a[2], b[0], tolerance);
}

template<typename Tol, typename T, std::size_t DerivationCount>
bool tolerantEquals(const P3D::Derivatives<T, DerivationCount>& first, const P3D::Derivatives<T, DerivationCount>& second, Tol tolerance) {
	for(std::size_t i = 0; i < DerivationCount; i++) {
		if(!tolerantEquals(first[i], second[i], tolerance)) return false;
	}
	return true;
}

template<typename Tol, typename T, std::size_t DerivationCount>
bool tolerantEquals(const P3D::TaylorExpansion<T, DerivationCount>& first, const P3D::TaylorExpansion<T, DerivationCount>& second, Tol tolerance) {
	return tolerantEquals(first.derivs, second.derivs, tolerance);
}

template<typename Tol, typename T, std::size_t DerivationCount>
bool tolerantEquals(const P3D::FullTaylorExpansion<T, DerivationCount>& first, const P3D::FullTaylorExpansion<T, DerivationCount>& second, Tol tolerance) {
	return tolerantEquals(first.derivs, second.derivs, tolerance);
}

template<typename Tol>
bool tolerantEquals(const P3D::TranslationalMotion& first, const P3D::TranslationalMotion& second, Tol tolerance) {
	return tolerantEquals(first.translation, second.translation, tolerance);
}

template<typename Tol>
bool tolerantEquals(const P3D::RotationalMotion& first, const P3D::RotationalMotion& second, Tol tolerance) {
	return tolerantEquals(first.rotation, second.rotation, tolerance);
}

template<typename Tol>
bool tolerantEquals(const P3D::Motion& first, const P3D::Motion& second, Tol tolerance) {
	return tolerantEquals(first.translation, second.translation, tolerance)
		&& tolerantEquals(first.rotation, second.rotation, tolerance);
}

template<typename Tol>
bool tolerantEquals(const P3D::RelativeMotion& first, const P3D::RelativeMotion& second, Tol tolerance) {
	return tolerantEquals(first.relativeMotion, second.relativeMotion, tolerance) &&
		tolerantEquals(first.locationOfRelativeMotion, second.locationOfRelativeMotion, tolerance);
}

template<typename Tol>
bool tolerantEquals(const P3D::BoundingBox& first, const P3D::BoundingBox& second, Tol tolerance) {
	return tolerantEquals(first.min, second.min, tolerance) &&
		tolerantEquals(first.max, second.max, tolerance);
}

template<typename T1, typename T2, typename Tol>
bool tolerantEquals(const std::pair<T1, T2>& first, const std::pair<T1, T2>& second, Tol tolerance) {
	return tolerantEquals(first.first, second.first, tolerance) &&
		tolerantEquals(first.second, second.second, tolerance);
}

template<typename T, std::size_t Size, typename Tol>
bool tolerantEquals(const std::array<T, Size>& first, const std::array<T, Size>& second, Tol tolerance) {
	for(std::size_t i = 0; i < Size; i++) {
		if(!tolerantEquals(first[i], second[i], tolerance)) {
			return false;
		}
	}
	return true;
}

template<typename T, typename Tol>
bool tolerantEquals(const std::vector<T>& first, const std::vector<T>& second, Tol tolerance) {
	if(first.size() != second.size()) throw std::logic_error("Incompatible vector sizes!");
	for(std::size_t i = 0; i < first.size(); i++) {
		if(!tolerantEquals(first[i], second[i], tolerance)) {
			return false;
		}
	}
	return true;
}

template<typename T, std::size_t Height, std::size_t Width, typename Tol>
bool tolerantEquals(const P3D::Matrix<T, Height, Width>& first, const P3D::UnmanagedHorizontalFixedMatrix<T, Width>& second, Tol tolerance) {
	assert(first.height() == second.height());
	for(size_t row = 0; row < Height; row++)
		for(size_t col = 0; col < Width; col++)
			if(!tolerantEquals(first(row, col), second(row, col), tolerance))
				return false;

	return true;
}
template<typename T, std::size_t Height, std::size_t Width, typename Tol>
bool tolerantEquals(const P3D::UnmanagedHorizontalFixedMatrix<T, Width>& second, const P3D::Matrix<T, Height, Width>& first, Tol tolerance) {
	return tolerantEquals(first, second, tolerance);
}
template<typename T, std::size_t Height, std::size_t Width, typename Tol>
bool tolerantEquals(const P3D::Matrix<T, Height, Width>& first, const P3D::UnmanagedVerticalFixedMatrix<T, Height>& second, Tol tolerance) {
	assert(first.width() == second.width());
	for(size_t row = 0; row < Height; row++)
		for(size_t col = 0; col < Width; col++)
			if(!tolerantEquals(first(row, col), second(row, col), tolerance))
				return false;

	return true;
}
template<typename T, std::size_t Height, std::size_t Width, typename Tol>
bool tolerantEquals(const P3D::UnmanagedVerticalFixedMatrix<T, Height>& second, const P3D::Matrix<T, Height, Width>& first, Tol tolerance) {
	return tolerantEquals(first, second, tolerance);
}
template<typename T, std::size_t Size, typename Tol>
bool tolerantEquals(const P3D::UnmanagedVerticalFixedMatrix<T, Size>& first, const P3D::UnmanagedVerticalFixedMatrix<T, Size>& second, Tol tolerance) {
	assert(first.width() == second.width());
	for(size_t row = 0; row < first.height(); row++)
		for(size_t col = 0; col < first.width(); col++)
			if(!tolerantEquals(first(row, col), second(row, col), tolerance))
				return false;

	return true;
}
template<typename T, std::size_t Size, typename Tol>
bool tolerantEquals(const P3D::UnmanagedHorizontalFixedMatrix<T, Size>& first, const P3D::UnmanagedHorizontalFixedMatrix<T, Size>& second, Tol tolerance) {
	assert(first.width() == second.width());
	for(size_t row = 0; row < first.height(); row++)
		for(size_t col = 0; col < first.width(); col++)
			if(!tolerantEquals(first(row, col), second(row, col), tolerance))
				return false;

	return true;
}
