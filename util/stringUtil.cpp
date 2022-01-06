#include "stringUtil.h"

#include <algorithm>
#include <vector>
#include <string>

namespace Util {

#ifdef __GNUG__
	#include <cstdlib>
	#include <memory>
	#include <cxxabi.h>

	std::string demangle(const std::string& fullName) {
		int status = 0;

		std::unique_ptr<char, void(*)(void*)> result {
			abi::__cxa_demangle(fullName.c_str(), NULL, NULL, &status),
			std::free
		};

		return (status == 0) ? result.get() : fullName;
	}

#elif _MSC_VER

std::string demangle(const std::string& fullName) {
	std::size_t colonIndex = fullName.find_last_of(':');
	std::string name = (colonIndex == std::string::npos) ? fullName.substr(fullName.find_last_of(' ') + 1) : fullName.substr(colonIndex + 1);

	return name;
}

#else

	std::string demangle(const std::string& fullName) {
		return fullName;
	}

#endif

std::string decamel(const std::string& string) {
	std::string result = string;

	if (result.empty())
		return result;

	for (std::size_t index = string.size() - 1; index >= 1; index--)
		if (isupper(string[index]))
			result.insert(index, " ");

	return result;
}

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

std::string until(const std::string& string, char end) {
	size_t l = string.length();
	for (size_t i = 0; i < l; i++) {
		if (string.at(i) == end)
			return string.substr(0, i);
	}

	return string;
}

std::string ltrim(const std::string& string) {
	std::string result = string;
	auto iterator = std::find_if(result.begin(),
	                             result.end(),
	                             [](int ch) {
		                             return ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r';
	                             });
	result.erase(result.begin(), iterator);
	return result;
}

std::string rtrim(const std::string& string) {
	std::string result = string;
	auto iterator = std::find_if(result.rbegin(),
	                             result.rend(),
	                             [](int ch) {
		                             return ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r';
	                             });
	result.erase(iterator.base(), result.end());
	return result;
}

std::string trim(const std::string& string) {
	return ltrim(rtrim(string));
}

};
