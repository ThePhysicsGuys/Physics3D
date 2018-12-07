#pragma once

#include "testsMain.h"

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
