#pragma once

#include "../engine/math/mat3.h"
#include "../engine/math/mat4.h"
#include "../engine/math/vec2.h"
#include "../engine/math/vec3.h"
#include "../engine/math/vec4.h"

template<typename Number>
bool tolerantEquals(const Number& first, const Number& second, Number tolerance) {
	Number diff = first - second;
	return diff <= tolerance && -diff <= tolerance;
}

template<typename Number>
bool tolerantNotEquals(const Number& first, const Number& second, Number tolerance) {
	Number diff = first - second;
	return diff > tolerance && -diff > tolerance;
}

template<typename Number>
bool tolerantLessThan(const Number& first, const Number& second, Number tolerance) {
	return first < second + tolerance;
}

template<typename Number>
bool tolerantGreaterThan(const Number& first, const Number& second, Number tolerance) {
	return first + tolerance > second;
}

template<typename Number>
bool tolerantLessOrEqual(const Number& first, const Number& second, Number tolerance) {
	return first <= second + tolerance;
}

template<typename Number>
bool tolerantGreaterOrEqual(const Number& first, const Number& second, Number tolerance) {
	return first + tolerance >= second;
}


template<typename Type>
bool tolerantEquals(const Mat3Template<Type>& first, const Mat3Template<Type>& second, Type tolerance) {
	for (int i = 0; i < 9; i++)
		if (!tolerantEquals(first.m[i], second.m[i], tolerance))
			return false;
	return true;
}
template<typename Type>
bool tolerantNotEquals(const Mat3Template<Type>& first, const Mat3Template<Type>& second, Type tolerance) {
	for (int i = 0; i < 9; i++)
		if (tolerantNotEquals(first.m[i], second.m[i], tolerance))
			return true;
	return false;
}

template<typename Type>
bool tolerantEquals(const Mat4Template<Type>& first, const Mat4Template<Type>& second, Type tolerance) {
	for (int i = 0; i < 16; i++)
		if (!tolerantEquals(first.m[i], second.m[i], tolerance))
			return false;
	return true;
}
template<typename Type>
bool tolerantNotEquals(const Mat4Template<Type>& first, const Mat4Template<Type>& second, Type tolerance) {
	for (int i = 0; i < 16; i++)
		if (tolerantNotEquals(first.m[i], second.m[i], tolerance))
			return true;
	return false;
}

template<typename Type>
bool tolerantEquals(const Vec2Template<Type>& first, const Vec2Template<Type>& second, Type tolerance) {
	return tolerantEquals(first.x, second.x, tolerance) &&
		tolerantEquals(first.y, second.y, tolerance);
}
template<typename Type>
bool tolerantNotEquals(const Vec2Template<Type>& first, const Vec2Template<Type>& second, Type tolerance) {
	return tolerantNotEquals(first.x, second.x, tolerance) ||
		tolerantNotEquals(first.y, second.y, tolerance);
}

template<typename Type>
bool tolerantEquals(const Vec3Template<Type>& first, const Vec3Template<Type>& second, Type tolerance) {
	return tolerantEquals(first.x, second.x, tolerance) &&
		tolerantEquals(first.y, second.y, tolerance) &&
		tolerantEquals(first.z, second.z, tolerance);
}
template<typename Type>
bool tolerantNotEquals(const Vec3Template<Type>& first, const Vec3Template<Type>& second, Type tolerance) {
	return tolerantNotEquals(first.x, second.x, tolerance) ||
		tolerantNotEquals(first.y, second.y, tolerance) ||
		tolerantNotEquals(first.z, second.z, tolerance);
}

template<typename Type>
bool tolerantEquals(const Vec4Template<Type>& first, const Vec4Template<Type>& second, Type tolerance) {
	return tolerantEquals(first.x, second.x, tolerance) &&
		tolerantEquals(first.y, second.y, tolerance) &&
		tolerantEquals(first.z, second.z, tolerance) &&
		tolerantEquals(first.w, second.w, tolerance);
}
template<typename Type>
bool tolerantNotEquals(const Vec4Template<Type>& first, const Vec4Template<Type>& second, Type tolerance) {
	return tolerantNotEquals(first.x, second.x, tolerance) ||
		tolerantNotEquals(first.y, second.y, tolerance) ||
		tolerantNotEquals(first.z, second.z, tolerance) ||
		tolerantNotEquals(first.w, second.w, tolerance);
}
