#pragma once

#include "../physics/templateUtils.h"

#include "../physics/math/linalg/vec.h"
#include "../physics/math/linalg/largeMatrix.h"
#include "../physics/math/linalg/mat.h"
#include "../physics/math/linalg/eigen.h"
#include "../physics/math/cframe.h"
#include "../physics/math/position.h"
#include "../physics/math/globalCFrame.h"
#include "../physics/motion.h"
#include "../physics/relativeMotion.h"

#include <utility>

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
bool tolerantEquals(const Vector<Num1, Size> & first, const Vector<Num2, Size>& second, Tol tolerance) {
	for (size_t i = 0; i < Size; i++) {
		if (!tolerantEquals(first[i], second[i], tolerance)) return false;
	}
	return true;
}
template<typename Num1, typename Num2, typename Tol>
bool tolerantEquals(const LargeVector<Num1>& first, const LargeVector<Num2>& second, Tol tolerance) {
	if(first.size != second.size) throw "Dimensions must match!";
	for(size_t i = 0; i < first.size; i++) {
		if(!tolerantEquals(first[i], second[i], tolerance)) return false;
	}
	return true;
}
template<typename Num1, typename Num2, typename Tol, size_t Width, size_t Height>
bool tolerantEquals(const Matrix<Num1, Width, Height>& first, const Matrix<Num2, Width, Height>& second, Tol tolerance) {
	for (size_t row = 0; row < Height; row++)
		for (size_t col = 0; col < Width; col++)
			if (!tolerantEquals(first[row][col], second[row][col], tolerance))
				return false;
	
	return true;
}

template<typename Num1, typename Num2, typename Tol, size_t Size>
bool tolerantEquals(const SymmetricMatrix<Num1, Size>& first, const SymmetricMatrix<Num2, Size>& second, Tol tolerance) {
	for (size_t row = 0; row < Size; row++)
		for (size_t col = 0; col < Size; col++)
			if (!tolerantEquals(first[row][col], second[row][col], tolerance))
				return false;

	return true;
}

template<typename Num1, typename Num2, typename Tol, size_t Size>
bool tolerantEquals(const DiagonalMatrix<Num1, Size>& first, const DiagonalMatrix<Num2, Size>& second, Tol tolerance) {
	for (size_t i = 0; i < Size; i++)
		if (!tolerantEquals(first[i], second[i], tolerance))
			return false;

	return true;
}

template<typename Num1, typename Num2, typename Tol>
bool tolerantEquals(const LargeMatrix<Num1>& first, const LargeMatrix<Num2>& second, Tol tolerance) {
	if(first.width != second.width || first.height != second.height) throw "Dimensions must match!";
	for(size_t row = 0; row < first.height; row++)
		for(size_t col = 0; col < first.width; col++)
			if(!tolerantEquals(first.get(row, col), second.get(row, col), tolerance))
				return false;

	return true;
}

template<typename Num1, typename Num2, typename Tol>
bool tolerantEquals(const LargeSymmetricMatrix<Num1>& first, const LargeSymmetricMatrix<Num2>& second, Tol tolerance) {
	if(first.size != second.size) throw "Dimensions must match!";
	for(size_t row = 0; row < first.size; row++)
		for(size_t col = row; col < first.size; col++)
			if(!tolerantEquals(first.get(row, col), second.get(row, col), tolerance))
				return false;

	return true;
}

template<typename Tol>
bool tolerantEquals(const CFrame& first, const CFrame& second, Tol tolerance) {
	return tolerantEquals(first.position, second.position, tolerance) &&
		tolerantEquals(first.rotation, second.rotation, tolerance);
}

template<typename Tol>
bool tolerantEquals(const Position& a, const Position& b, Tol tolerance) {
	Vec3 delta = a - b;
	return tolerantEquals(delta, Vec3(0, 0, 0), tolerance);
}

template<typename Tol>
bool tolerantEquals(const GlobalCFrame& first, const GlobalCFrame& second, Tol tolerance) {
	return tolerantEquals(first.position, second.position, tolerance) &&
		tolerantEquals(first.rotation, second.rotation, tolerance);
}

template<typename Tol, typename N>
bool tolerantEquals(const EigenValues<N, 3>& a, const EigenValues<N, 3>& b, Tol tolerance) {
	return tolerantEquals(a[0], b[0], tolerance) && tolerantEquals(a[1], b[1], tolerance) && tolerantEquals(a[2], b[2], tolerance) ||
		tolerantEquals(a[0], b[0], tolerance) && tolerantEquals(a[1], b[2], tolerance) && tolerantEquals(a[2], b[1], tolerance) ||
		tolerantEquals(a[0], b[1], tolerance) && tolerantEquals(a[1], b[0], tolerance) && tolerantEquals(a[2], b[2], tolerance) ||
		tolerantEquals(a[0], b[1], tolerance) && tolerantEquals(a[1], b[2], tolerance) && tolerantEquals(a[2], b[0], tolerance) ||
		tolerantEquals(a[0], b[2], tolerance) && tolerantEquals(a[1], b[0], tolerance) && tolerantEquals(a[2], b[1], tolerance) ||
		tolerantEquals(a[0], b[2], tolerance) && tolerantEquals(a[1], b[1], tolerance) && tolerantEquals(a[2], b[0], tolerance);
}

template<typename Tol>
bool tolerantEquals(const Motion& first, const Motion& second, Tol tolerance) {
	for(int i = 0; i < 4; i++) {
		if(!tolerantEquals(first.components[i], second.components[i], tolerance)) return false;
	}
	return true;
}

template<typename Tol>
bool tolerantEquals(const RelativeMotion& first, const RelativeMotion& second, Tol tolerance) {
	return tolerantEquals(first.relativeMotion, second.relativeMotion, tolerance) && 
		tolerantEquals(first.locationOfRelativeMotion, second.locationOfRelativeMotion, tolerance);
}

template<typename T1, typename T2, typename Tol>
bool tolerantEquals(const std::pair<T1, T2>& first, const std::pair<T1, T2>& second, Tol tolerance) {
	return tolerantEquals(first.first, second.first, tolerance) && 
		tolerantEquals(first.second, second.second, tolerance);
}
