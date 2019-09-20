#pragma once

#include "../engine/math/linalg/vec.h"
#include "../engine/math/linalg/mat.h"
#include "../engine/templateUtils.h"
#include "../engine/math/cframe.h"

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
template<typename Num1, typename Num2, typename Tol, size_t Size>
bool tolerantNotEquals(const Vector<Num1, Size>& first, const Vector<Num2, Size>& second, Tol tolerance) {
	for (size_t i = 0; i < Size; i++) {
		if (tolerantNotEquals(first[i], second[i], tolerance)) return true;
	}
	return false;
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

template<typename Num1, typename Num2, typename Tol, size_t Width, size_t Height>
bool tolerantNotEquals(const Matrix<Num1, Width, Height>& first, const Matrix<Num2, Width, Height>& second, Tol tolerance) {
	for (size_t row = 0; row < Height; row++)
		for (size_t col = 0; col < Width; col++)
			if (tolerantNotEquals(first[row][col], second[row][col], tolerance))
				return true;
	
	return false;
}

template<typename Num1, typename Num2, typename Tol, size_t Size>
bool tolerantNotEquals(const SymmetricMatrix<Num1, Size>& first, const SymmetricMatrix<Num2, Size>& second, Tol tolerance) {
	for (size_t row = 0; row < Size; row++)
		for (size_t col = 0; col < Size; col++)
			if (tolerantNotEquals(first[row][col], second[row][col], tolerance))
				return true;

	return false;
}

template<typename Num1, typename Num2, typename Tol, size_t Size>
bool tolerantNotEquals(const DiagonalMatrix<Num1, Size>& first, const DiagonalMatrix<Num2, Size>& second, Tol tolerance) {
	for (size_t i = 0; i < Size; i++)
		if (tolerantNotEquals(first[i], second[i], tolerance))
			return true;

	return false;
}

template<typename Tol>
bool tolerantEquals(const CFrame& first, const CFrame& second, Tol tolerance) {
	return tolerantEquals(first.position, second.position, tolerance) &&
		tolerantEquals(first.rotation, second.rotation, tolerance);
}

template<typename Tol>
bool tolerantNotEquals(const CFrame& first, const CFrame& second, Tol tolerance) {
	return tolerantNotEquals(first.position, second.position, tolerance) ||
		tolerantNotEquals(first.rotation, second.rotation, tolerance);
}

/*template<typename Tol, typename N>
bool tolerantEquals(const EigenValues<N>& a, const EigenValues<N>& b, Tol tolerance) {
	return tolerantEquals(a[0], b[0], tolerance) && tolerantEquals(a[1], b[1], tolerance) && tolerantEquals(a[2], b[2], tolerance) ||
		tolerantEquals(a[0], b[0], tolerance) && tolerantEquals(a[1], b[2], tolerance) && tolerantEquals(a[2], b[1], tolerance) ||
		tolerantEquals(a[0], b[1], tolerance) && tolerantEquals(a[1], b[0], tolerance) && tolerantEquals(a[2], b[2], tolerance) ||
		tolerantEquals(a[0], b[1], tolerance) && tolerantEquals(a[1], b[2], tolerance) && tolerantEquals(a[2], b[0], tolerance) ||
		tolerantEquals(a[0], b[2], tolerance) && tolerantEquals(a[1], b[0], tolerance) && tolerantEquals(a[2], b[1], tolerance) ||
		tolerantEquals(a[0], b[2], tolerance) && tolerantEquals(a[1], b[1], tolerance) && tolerantEquals(a[2], b[0], tolerance);
}
*/