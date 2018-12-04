#pragma once

#include "testsMain.h"

#include <sstream>

template<typename R, typename P>
std::string errMsg(const R& first, const P& second, const char* sep) {
	std::stringstream s;
	s << "(";
	s << first;
	s << ' ';
	s << sep;
	s << ' ';
	s << second;
	s << ")";
	return s.str();
}

template<typename R>
std::string errMsg(const R& first) {
	std::stringstream s;
	s << "(";
	s << first;
	s << ")";
	return s.str();
}

template<typename T>
class AssertComparer {
public:
	int line;
	const T& arg;

	AssertComparer(int line, const T& arg) : line(line), arg(arg) {}

	template<typename P> bool operator<(const P& other) const { if (!(arg < other)) { throw AssertionError(line, errMsg(arg, other, "<")); }; return true; }
	template<typename P> bool operator>(const P& other) const { if (!(arg > other)) { throw AssertionError(line, errMsg(arg, other, ">")); }; return true; }
	template<typename P> bool operator<=(const P& other) const { if (!(arg <= other)) { throw AssertionError(line, errMsg(arg, other, "<=")); }; return true; }
	template<typename P> bool operator>=(const P& other) const { if (!(arg >= other)) { throw AssertionError(line, errMsg(arg, other, ">=")); }; return true; }
	template<typename P> bool operator==(const P& other) const { if (!(arg == other)) { throw AssertionError(line, errMsg(arg, other, "==")); }; return true; }
	template<typename P> bool operator!=(const P& other) const { if (!(arg != other)) { throw AssertionError(line, errMsg(arg, other, "!=")); }; return true; }
	bool operator~() const { if (!(arg)) throw AssertionError(line, errMsg(arg)); return true; }
};

struct AssertBuilder {
	int line;
	AssertBuilder(int line) : line(line) {};
	template<typename T>
	AssertComparer<T> operator<(const T& other) const { return AssertComparer<T>(line, other); }
};

#define ASSERT(condition) ~(AssertBuilder(__LINE__) < condition)
