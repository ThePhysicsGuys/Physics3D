#pragma once

#include "mat3.h"
#include "mat4.h"
#include "vec3.h"
#include "vec2.h"

#include <string>
#include <sstream>

template<typename N>
inline std::ostream& operator<<(std::ostream& os, const Mat4Template<N>& matrix) {
	os << "Mat4(\n\t";
	for (int i = 0; i < 15; i++) {
		os << matrix.m[i];
		os << (((i + 1) % 4 == 0) ? ";\n\t" : ", ");
	}
	os << matrix.m[15] << ")";

	return os;
}

template<typename N>
inline std::string str(const Mat4Template<N>& matrix) {
	std::stringstream ss;
	ss << matrix;
	return ss.str();
}

template<typename N>
inline std::ostream& operator<<(std::ostream& os, const Mat3Template<N>& matrix) {
	os << "Mat3(\n\t";

	for (int i = 0; i < 8; i++) {
		os << matrix.m[i];
		os << (((i + 1) % 3 == 0) ? ";\n\t" : ", ");
	}
	os << matrix.m[8] << ")";

	return os;
}

template<typename N>
inline std::string str(const Mat3Template<N>& matrix) {
	std::stringstream ss;
	ss << matrix;
	return ss.str();
}

template<typename N>
inline std::ostream& operator<<(std::ostream& os, const Vec3Template<N>& vector) {
	os << "Vec3(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
	return os;
}

template<typename N>
inline std::string str(const Vec3Template<N>& vector) {
	std::stringstream ss;
	ss << vector;
	return ss.str();
}

template<typename N>
inline std::ostream& operator<<(std::ostream& os, const Vec2Template<N>& vector) {
	os << "Vec2(" << vector.x << ", " << vector.y << ")";
	return os;
}

template<typename N>
inline std::string str(const Vec2Template<N>& vector) {
	std::stringstream ss;
	ss << vector;
	return ss.str();
}


std::ostream& operator<<(std::ostream& os, const CFrame& cframe);
std::string str(const CFrame& cframe);
