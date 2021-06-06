#pragma once

#include <exception>
#include <string>

namespace P3D {
class assertion_error : public std::exception {
	const char* file;
	int line;
public:
	assertion_error(const char* file, int line) : std::exception(), file(file), line(line) {}
};

#ifndef NDEBUG
#define catchable_assert(condition) if(!(condition)) throw assertion_error(__FILE__, __LINE__)
#else
#define catchable_assert(condition)
#endif
};
