#include "testsMain.h"

#include "../application/gui/loader.h"

#include <string>
#include <vector>

std::vector<std::string> split(std::string &string, char splitter) {
	std::vector<std::string> elements;
	int length = string.length();
	int start = 0;
	for (int i = 0; i < length; i++) {
		if (string[i] == splitter) {
			if (i != start) elements.push_back(string.substr(start, i - start));
			start = i + 1;
		}
	}
	if (start < length) elements.push_back(string.substr(start, length - start));
	return elements;
}

TEST_CASE(splitTest) {
	std::string testString = "/this is/a test /";
	std::vector<std::string> testVector = split(testString, '/');
	for (std::string s : testVector) {
		Log::fatal("%s", s.c_str());
	}
}
