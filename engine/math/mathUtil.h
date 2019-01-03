#pragma once

#include "mat3.h"
#include "mat4.h"
#include "vec4.h"
#include "vec3.h"
#include "vec2.h"

#include <string>
#include <sstream>

template<typename N>
std::ostream& operator<<(std::ostream& os, const Mat4Template<N>& matrix) {
	os << "Mat4Template(\n\t";
	for (int i = 0; i < 15; i++) {
		os << matrix.m[i];
		os << (((i + 1) % 4 == 0) ? ";\n\t" : ", ");
	}
	os << matrix.m[15] << ")";

	return os;
}

template<typename N>
std::string str(const Mat4Template<N>& matrix) {
	std::stringstream ss;
	ss << "Mat4Template(\n\t";
	for (int i = 0; i < 15; i++) {
		ss << matrix.m[i];
		ss << (((i + 1) % 4 == 0) ? ";\n\t" : ", ");
	}
	ss << matrix.m[15] << ")";

	return ss.str();
}

template<typename N>
std::ostream& operator<<(std::ostream& os, const Mat3Template<N>& matrix) {
	os << "Mat3Template(\n\t";

	for (int i = 0; i < 8; i++) {
		os << matrix.m[i];
		os << (((i + 1) % 3 == 0) ? ";\n\t" : ", ");
	}
	os << matrix.m[8] << ")";

	return os;
}

template<typename N>
std::string str(const Mat3Template<N>& matrix) {
	std::stringstream ss;
	ss << "Mat3Template(\n\t";

	for (int i = 0; i < 8; i++) {
		ss << matrix.m[i];
		ss << (((i + 1) % 3 == 0) ? ";\n\t" : ", ");
	}
	ss << matrix.m[8] << ")";

	return ss.str();
}

template<typename N>
std::ostream& operator<<(std::ostream& os, const Vec4Template<N>& vector) {
	os << "Vec4Template(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";
	return os;
}

template<typename N>
std::string str(const Vec4Template<N>& vector) {
	std::stringstream ss;
	ss << "Vec4Template(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";
	return ss.str();
}


template<typename N>
std::ostream& operator<<(std::ostream& os, const Vec3Template<N>& vector) {
	os << "Vec3Template(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
	return os;
}

template<typename N>
std::string str(const Vec3Template<N>& vector) {
	std::stringstream ss;
	ss << "Vec3Template(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
	return ss.str();
}

template<typename N>
std::ostream& operator<<(std::ostream& os, const Vec2Template<N>& vector) {
	os << "Vec2Template(" << vector.x << ", " << vector.y << ")";
	return os;
}

template<typename N>
std::string str(const Vec2Template<N>& vector) {
	std::stringstream ss;
	ss << "Vec2Template(" << vector.x << ", " << vector.y << ")";
	return ss.str();
}
