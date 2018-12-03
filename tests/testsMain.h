#pragma once

#include <string>

#define JOIN_TEST_CASE2(a,b) a##b
#define JOIN_TEST_CASE(a,b) JOIN_TEST_CASE2(a,b)
#define TEST_CASE(func) void func(); static TestAdder JOIN_TEST_CASE(tAdder, __LINE__)(__FILE__, #func, func); void func()

struct TestAdder {
	TestAdder(const char* file, const char* name, void(*f)());
};

class AssertionError {
	std::string info;
public:
	int line;
	AssertionError(int line, std::string info);
	const char* what() const throw();
};
