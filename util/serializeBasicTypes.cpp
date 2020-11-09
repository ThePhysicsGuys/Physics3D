#include "serializeBasicTypes.h"

#include <sstream>

void serialize(const char* data, size_t size, std::ostream& ostream) {
	ostream.write(data, size);
}

void deserialize(char* buf, size_t size, std::istream& istream) {
	istream.read(buf, size);
}

template<>
void serialize<char>(const char& c, std::ostream& ostream) {
	ostream << c;
}
template<>
char deserialize<char>(std::istream& istream) {
	return istream.get();
}

template<>
void serialize<bool>(const bool& b, std::ostream& ostream) {
	serialize<char>(b ? 255 : 0, ostream);
}
template<>
bool deserialize<bool>(std::istream& istream) {
	return deserialize<char>(istream) != 0;
}

void serializeString(const std::string& str, std::ostream& ostream) {
	::serialize(str.c_str(), str.length(), ostream);
	::serialize<char>('\0', ostream);
}

std::string deserializeString(std::istream& istream) {
	std::stringstream sstream;

	while(char c = istream.get()) {
		sstream << c;
	}

	return sstream.str();
}
