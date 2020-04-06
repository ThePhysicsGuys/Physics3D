#include "stringUtil.h"

#include <algorithm>
#include <vector>
#include <string>

std::vector<std::string> split(const std::string& string, char splitter) {
	std::vector<std::string> elements;
	size_t length = string.size();
	size_t start = 0;

	for (size_t i = 0; i < length; i++) {
		if (string[i] == splitter) {
			elements.push_back(string.substr(start, i - start));
			start = i + 1;
		}
	}

	if (start < length)
		elements.push_back(string.substr(start, length - start));

	return elements;
}

bool startsWith(const std::string& string, const std::string& prefix) {
	size_t l1 = string.length();
	size_t l2 = prefix.length();

	if (l2 > l1)
		return false;

	for (size_t i = 0; i < l2; i++) {
		if (string[i] != prefix[i])
			return false;
	}

	return true;
}

bool endsWith(const std::string& string, const std::string& suffix) {
	size_t l1 = string.length();
	size_t l2 = suffix.length();

	if (l2 > l1)
		return false;

	for (size_t i = 1; i <= l2; i++) {
		if (string[l1 - i] != suffix[l2 - i])
			return false;
	}

	return true;
}

std::string ltrim(std::string string) {
	string.erase(string.begin(), std::find_if(string.begin(), string.end(), [](int ch) {
		return ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r';
	}));
	return string;
}

std::string rtrim(std::string string) {
	string.erase(std::find_if(string.rbegin(), string.rend(), [](int ch) {
		return ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r';
	}).base(), string.end());
	return string;
}

std::string trim(std::string string) {
	return ltrim(rtrim(string));
}