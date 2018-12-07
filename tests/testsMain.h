#pragma once

#define JOIN_TEST_CASE2(a,b) a##b
#define JOIN_TEST_CASE(a,b) JOIN_TEST_CASE2(a,b)
#define TEST_CASE(func) void func(); static TestAdder JOIN_TEST_CASE(tAdder, __LINE__)(__FILE__, #func, func); void func()

struct TestAdder {
	TestAdder(const char* file, const char* name, void(*f)());
};

class AssertionError {
	const char* info;
public:
	int line;
	AssertionError(int line, const char* info);
	const char* what() const throw();
};
