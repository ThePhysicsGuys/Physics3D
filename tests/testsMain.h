#pragma once

#include "compare.h"
#include <sstream>

class TestInterface {
	size_t assertCount = 0;
public:
	bool debugOnFailure;
	TestInterface() : debugOnFailure(false) {}
	TestInterface(bool debugOnFailure) : debugOnFailure(debugOnFailure) {}
	inline void markAssert() {
		assertCount++;
	}
	inline size_t getAssertCount() const { return assertCount; }
};

enum class TestType {
	NORMAL,
	SLOW
};

#define __JOIN2(a,b) a##b
#define __JOIN(a,b) __JOIN2(a,b)
#define TEST_CASE(func) void func(); static TestAdder __JOIN(tAdder, __LINE__)(__FILE__, #func, func, TestType::NORMAL); void func()
#define TEST_CASE_SLOW(func) void func(); static TestAdder __JOIN(tAdder, __LINE__)(__FILE__, #func, func, TestType::SLOW); void func()

struct TestAdder {
	TestAdder(const char* filePath, const char* nameName, void(*testFunc)(), TestType isSlow);
};

class AssertionError {
	const char* info;
public:
	int line;
	AssertionError(int line, const char* info);
	const char* what() const noexcept;
};

void logf(const char* format, ...);
extern std::stringstream logStream;
extern thread_local TestInterface __testInterface;
extern const bool reffableTrue;

// Testing utils:

template<typename T, std::size_t Size>
std::ostream& operator<<(std::ostream& ostream, const std::array<T, Size>& arr) {
	ostream << '{' << arr[0];
	for(std::size_t i = 1; i < Size; i++) {
		ostream << ", " << arr[i];
	}
	ostream << '}';
	return ostream;
}

template<typename R, typename P>
const char* errMsg(const R& first, const P& second, const char* sep) {
	std::stringstream s;
	s << first;
	s << ' ';
	s << sep;
	s << '\n';
	s << second;

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


#ifdef _MSC_VER
#define __DEBUG_BREAK __debugbreak();
#else
#define __DEBUG_BREAK
#endif
#define __ASSERT_FAILURE(line, printText) if(__testInterface.debugOnFailure) {__DEBUG_BREAK} throw AssertionError(line, printText)

template<typename T>
class AssertComparer {
public:
	const int line;
	const T& arg;

	AssertComparer(int line, const T& arg) : line(line), arg(arg) {}

	template<typename P> AssertComparer<bool> operator<(const P& other) const { if (!(arg < other)) { __ASSERT_FAILURE(line, errMsg(arg, other, "<")); }; return AssertComparer<bool>(this->line, reffableTrue); }
	template<typename P> AssertComparer<bool> operator>(const P& other) const { if (!(arg > other)) { __ASSERT_FAILURE(line, errMsg(arg, other, ">")); }; return AssertComparer<bool>(this->line, reffableTrue); }
	template<typename P> AssertComparer<bool> operator<=(const P& other) const { if (!(arg <= other)) { __ASSERT_FAILURE(line, errMsg(arg, other, "<=")); }; return AssertComparer<bool>(this->line, reffableTrue); }
	template<typename P> AssertComparer<bool> operator>=(const P& other) const { if (!(arg >= other)) { __ASSERT_FAILURE(line, errMsg(arg, other, ">=")); }; return AssertComparer<bool>(this->line, reffableTrue); }
	template<typename P> AssertComparer<bool> operator==(const P& other) const { if (!(arg == other)) { __ASSERT_FAILURE(line, errMsg(arg, other, "==")); }; return AssertComparer<bool>(this->line, reffableTrue); }
	template<typename P> AssertComparer<bool> operator!=(const P& other) const { if (!(arg != other)) { __ASSERT_FAILURE(line, errMsg(arg, other, "!=")); }; return AssertComparer<bool>(this->line, reffableTrue); }
};

template<typename T, typename Tol>
class TolerantAssertComparer {
public:
	const int line;
	const T& arg;
	const Tol tolerance;

	TolerantAssertComparer(int line, const T& arg, Tol tolerance) : line(line), arg(arg), tolerance(tolerance) {}

	template<typename T2> TolerantAssertComparer<bool, Tol> operator<(const T2& other) const { if(!tolerantLessThan(arg, other, tolerance)) { __ASSERT_FAILURE(line, errMsg(arg, other, "<")); }; return TolerantAssertComparer<bool, Tol>(this->line, reffableTrue, this->tolerance); }
	template<typename T2> TolerantAssertComparer<bool, Tol> operator>(const T2& other) const { if (!tolerantGreaterThan(arg, other, tolerance)) { __ASSERT_FAILURE(line, errMsg(arg, other, ">")); }; return TolerantAssertComparer<bool, Tol>(this->line, reffableTrue, this->tolerance); }
	template<typename T2> TolerantAssertComparer<bool, Tol> operator<=(const T2& other) const { if (!tolerantLessOrEqual(arg, other, tolerance)) { __ASSERT_FAILURE(line, errMsg(arg, other, "<=")); }; return TolerantAssertComparer<bool, Tol>(this->line, reffableTrue, this->tolerance); }
	template<typename T2> TolerantAssertComparer<bool, Tol> operator>=(const T2& other) const { if (!tolerantGreaterOrEqual(arg, other, tolerance)) { __ASSERT_FAILURE(line, errMsg(arg, other, ">=")); }; return TolerantAssertComparer<bool, Tol>(this->line, reffableTrue, this->tolerance); }
	template<typename T2> TolerantAssertComparer<bool, Tol> operator==(const T2& other) const { if (!tolerantEquals(arg, other, tolerance)) { __ASSERT_FAILURE(line, errMsg(arg, other, "==")); }; return TolerantAssertComparer<bool, Tol>(this->line, reffableTrue, this->tolerance); }
	template<typename T2> TolerantAssertComparer<bool, Tol> operator!=(const T2& other) const { if (!tolerantNotEquals(arg, other, tolerance)) { __ASSERT_FAILURE(line, errMsg(arg, other, "!=")); }; return TolerantAssertComparer<bool, Tol>(this->line, reffableTrue, this->tolerance); }
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

#define ASSERT_STRICT(condition) do {if(!(AssertBuilder(__LINE__) < condition).arg) {__ASSERT_FAILURE(__LINE__, "false");}__testInterface.markAssert(); }while(false)
#define ASSERT_TOLERANT(condition, tolerance) do {if(!(TolerantAssertBuilder<decltype(tolerance)>(__LINE__, tolerance) < condition).arg) {__ASSERT_FAILURE(__LINE__, "false");} __testInterface.markAssert(); }while(false)
#define ASSERT_TRUE(condition) do {if(!(condition)) {__ASSERT_FAILURE(__LINE__, "false");}__testInterface.markAssert(); }while(false)
#define ASSERT_FALSE(condition) do {if(condition) {__ASSERT_FAILURE(__LINE__, "true");}__testInterface.markAssert(); }while(false)

#define PREV_VAL_NAME __JOIN(____previousValue, __LINE__)
#define ISFILLED_NAME __JOIN(____isFilled, __LINE__)
#define REMAINS_CONSTANT_STRICT(value) do{\
	static bool ISFILLED_NAME = false;\
	static auto PREV_VAL_NAME = value;\
	if(ISFILLED_NAME) ASSERT_STRICT(PREV_VAL_NAME == (value));\
	ISFILLED_NAME = true;\
}while(false)
#define REMAINS_CONSTANT_TOLERANT(value, tolerance) do{\
	static bool ISFILLED_NAME = false;\
	static auto PREV_VAL_NAME = value;\
	if(ISFILLED_NAME) ASSERT_TOLERANT(PREV_VAL_NAME == (value), tolerance);\
	ISFILLED_NAME = true;\
}while(false)
