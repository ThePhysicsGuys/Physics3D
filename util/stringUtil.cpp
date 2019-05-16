#include "stringUtil.h"

#include <algorithm>

std::vector<std::string> split(const std::string& string, char splitter) {
	std::vector<std::string> elements;
	int length = string.size();
	int start = 0;

	for (int i = 0; i < length; i++) {
		if (string[i] == splitter) {
			if (i != start)
				elements.push_back(string.substr(start, i - start));
			start = i + 1;
		}
	}

	if (start < length)
		elements.push_back(string.substr(start, length - start));

	return elements;
}

bool startWith(std::string string, std::string prefix) {
	if (string.length() < prefix.length())
		return false;

	for (int i = 0; i < prefix.length(); i++) {
		if (string[i] != prefix[i])
			return false;
	}

	return true;
}

std::string ltrim(std::string string) {
	string.erase(string.begin(), std::find_if(string.begin(), string.end(), [](int ch) {
		return ch != ' ';
		}));
	return string;
}

std::string rtrim(std::string string) {
	string.erase(std::find_if(string.rbegin(), string.rend(), [](int ch) {
		return ch != ' ';
		}).base(), string.end());
	return string;
}

std::string trim(std::string string) {
	return ltrim(rtrim(string));
}