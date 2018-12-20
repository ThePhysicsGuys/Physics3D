#pragma once

#include "testsMain.h"

#include "compare.h"

#include <sstream>

template<typename R, typename P>
const char* errMsg(const R& first, const P& second, const char* sep) {
	std::stringstream s;
	s << "(";
	s << first;
	s << ' ';
	s << sep;
	s << ' ';
	s << second;
	s << ")";

	std::string msg = s.str();

	const char* data = msg.c_str();
	char* dataBuf = new char[msg.size() + 1];
	for (int i = 0; i < msg.size() + 1; i++)
		dataBuf[i] = data[i];

	return dataBuf;
}

template<typename R>
const char* errMsg(const R& first) {
	std::stringstream s;
	s << "(";
	s << first;
	s << ")";
	std::string msg = s.str();

	const char* data = msg.c_str();
	char* dataBuf = new char[msg.size() + 1];
	for (int i = 0; i < msg.size() + 1; i++) 
		dataBuf[i] = data[i];
	
	return dataBuf;
}

template<typename T>
class AssertComparer {
public:
	const int line;
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

template<typename T, typename Tol>
class TolerantAssertComparer {
public:
	const int line;
	const T& arg;
	const Tol tolerance;

	TolerantAssertComparer(int line, const T& arg, Tol tolerance) : line(line), arg(arg), tolerance(tolerance) {}

	template<typename P> bool operator<(const P& other) const { if (!tolerantLessThan(arg, other, tolerance)) { throw AssertionError(line, errMsg(arg, other, "<")); }; return true; }
	template<typename P> bool operator>(const P& other) const { if (!tolerantGreaterThan(arg, other, tolerance)) { throw AssertionError(line, errMsg(arg, other, ">")); }; return true; }
	template<typename P> bool operator<=(const P& other) const { if (!tolerantLessOrEqual(arg, other, tolerance)) { throw AssertionError(line, errMsg(arg, other, "<=")); }; return true; }
	template<typename P> bool operator>=(const P& other) const { if (!tolerantGreaterOrEqual(arg, other, tolerance)) { throw AssertionError(line, errMsg(arg, other, ">=")); }; return true; }
	template<typename P> bool operator==(const P& other) const { if (!tolerantEquals(arg, other, tolerance)) { throw AssertionError(line, errMsg(arg, other, "==")); }; return true; }
	template<typename P> bool operator!=(const P& other) const { if (!tolerantNotEquals(arg, other, tolerance)) { throw AssertionError(line, errMsg(arg, other, "!=")); }; return true; }
	bool operator~() const { if (!(arg)) throw AssertionError(line, errMsg(arg)); return true; }
};

struct AssertBuilder {
	int line;
	AssertBuilder(int line) : line(line) {};
	template<typename T>
	AssertComparer<T> operator<(const T& other) const { return AssertComparer<T>(line, other); }
};

template<typename Tol>
struct TolerantAssertBuilder {
	int line;
	Tol tolerance;
	TolerantAssertBuilder(int line, Tol tolerance) : line(line), tolerance(tolerance) {};
	template<typename T>
	TolerantAssertComparer<T, Tol> operator<(const T& other) const { return TolerantAssertComparer<T, Tol>(line, other, tolerance); }
};

#define ASSERT_STRICT(condition) ~(AssertBuilder(__LINE__) < condition)
#define ASSERT_TOLERANT(condition, tolerance) ~(TolerantAssertBuilder<decltype(tolerance)>(__LINE__, tolerance) < condition)
#define __ASSERT_PICK(_1, _2, NAME, ...) NAME

#ifdef _MSC_VER
#define __ASSERT__EXPAND(x) x
#define ASSERT(...) __ASSERT__EXPAND(__ASSERT_PICK(__VA_ARGS__, ASSERT_TOLERANT, ASSERT_STRICT)(__VA_ARGS__))
#else
#define ASSERT(...) __ASSERT_PICK(__VA_ARGS__, ASSERT_TOLERANT, ASSERT_STRICT)(__VA_ARGS__)
#endif
