#pragma once

#include "compare.h"
#include <sstream>

#define __JOIN2(a,b) a##b
#define __JOIN(a,b) __JOIN2(a,b)
#define TEST_CASE(func) void func(); static TestAdder __JOIN(tAdder, __LINE__)(__FILE__, #func, func); void func()

struct TestAdder {
	TestAdder(const char* filePath, const char* nameName, void(*testFunc)());
};

class AssertionError {
	const char* info;
public:
	int line;
	AssertionError(int line, const char* info);
	const char* what() const throw();
};

extern std::stringstream logStream;
void logf(const char* format, ...);

// Testing utils:

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
};

template<typename T, typename Tol>
class TolerantAssertComparer {
public:
	const int line;
	const T& arg;
	const Tol tolerance;

	TolerantAssertComparer(int line, const T& arg, Tol tolerance) : line(line), arg(arg), tolerance(tolerance) {}

	template<typename T> bool operator<( const T& other) const { if (!tolerantLessThan(arg, other, tolerance)) { throw AssertionError(line, errMsg(arg, other, "<")); }; return true; }
	template<typename T> bool operator>( const T& other) const { if (!tolerantGreaterThan(arg, other, tolerance)) { throw AssertionError(line, errMsg(arg, other, ">")); }; return true; }
	template<typename T> bool operator<=(const T& other) const { if (!tolerantLessOrEqual(arg, other, tolerance)) { throw AssertionError(line, errMsg(arg, other, "<=")); }; return true; }
	template<typename T> bool operator>=(const T& other) const { if (!tolerantGreaterOrEqual(arg, other, tolerance)) { throw AssertionError(line, errMsg(arg, other, ">=")); }; return true; }
	template<typename T> bool operator==(const T& other) const { if (!tolerantEquals(arg, other, tolerance)) { throw AssertionError(line, errMsg(arg, other, "==")); }; return true; }
	template<typename T> bool operator!=(const T& other) const { if (!tolerantNotEquals(arg, other, tolerance)) { throw AssertionError(line, errMsg(arg, other, "!=")); }; return true; }
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

#define ASSERT_STRICT(condition) (AssertBuilder(__LINE__) < condition)
#define ASSERT_TOLERANT(condition, tolerance) (TolerantAssertBuilder<decltype(tolerance)>(__LINE__, tolerance) < condition)
#define ASSERT_TRUE(condition) if(!(condition)) throw AssertionError(__LINE__, "False")
#define ASSERT_FALSE(condition) if(condition) throw AssertionError(__LINE__, "True")

#define PREV_VAL_NAME __JOIN(____previousValue, __LINE__)
#define ISFILLED_NAME __JOIN(____isFilled, __LINE__)
#define REMAINS_CONSTANT_STRICT(value) {\
	static bool ISFILLED_NAME = false;\
	static auto PREV_VAL_NAME = value;\
	if(ISFILLED_NAME) ASSERT_STRICT(PREV_VAL_NAME == (value));\
	ISFILLED_NAME = true;\
}
#define REMAINS_CONSTANT_TOLERANT(value, tolerance) {\
	static bool ISFILLED_NAME = false;\
	static auto PREV_VAL_NAME = value;\
	if(ISFILLED_NAME) ASSERT_TOLERANT(PREV_VAL_NAME == (value), tolerance);\
	ISFILLED_NAME = true;\
}
