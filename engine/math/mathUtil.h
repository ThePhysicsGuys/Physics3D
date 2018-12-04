#pragma once

#include "mat3.h"
#include "mat4.h"

#include <string>
#include <sstream>

template<typename N>
std::ostream& operator<<(std::ostream& os, const Mat4Template<N>& matrix) {
	os << "Mat4Template(";
	for (int i = 0; i < 15; i++) {
		os << matrix.m[i];
		os << (((i + 1) % 4 == 0) ? "; " : ", ");
	}
	os << matrix.m[15] << ")";

	return os;
}

template<typename N>
std::string str(Mat4Template<N>& matrix) {
	std::stringstream ss;
	ss << std::string("Mat4Template(\n\t");
	for (int i = 0; i < 15; i++) {
		ss << matrix.m[i];
		ss << (((i + 1) % 4 == 0) ? ";\n\t" : ", ");
	}
	ss << matrix.m[15] << ")";

	return ss.str();
}

template<typename N>
std::ostream& operator<<(std::ostream& os, const Mat3Template<N>& matrix) {
	os << "Mat3Template(";

	for (int i = 0; i < 8; i++) {
		os << matrix.m[i];
		os << (((i + 1) % 3 == 0) ? "; " : ", ");
	}
	os << matrix.m[8] << ")";

	return os;
}

template<typename N>
std::string str(Mat3Template<N>& matrix) {
	std::stringstream ss;
	ss << std::string("Mat3Template(");

	for (int i = 0; i < 8; i++) {
		ss << matrix.m[i];
		ss << (((i + 1) % 3 == 0) ? "; " : ", ");
	}
	ss << matrix.m[8] << ")";

	return ss.str();
}
