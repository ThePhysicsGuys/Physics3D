#include "serializeBasicTypes.h"

#include <sstream>

namespace P3D {
void serializeBasicTypes(const char* data, size_t size, std::ostream& ostream) {
	ostream.write(data, size);
}

void deserializeBasicTypes(char* buf, size_t size, std::istream& istream) {
	istream.read(buf, size);
}

template<>
void serializeBasicTypes<char>(const char& c, std::ostream& ostream) {
	ostream << c;
}
template<>
char deserializeBasicTypes<char>(std::istream& istream) {
	return istream.get();
}

template<>
void serializeBasicTypes<bool>(const bool& b, std::ostream& ostream) {
	serializeBasicTypes<char>(b ? 255 : 0, ostream);
}
template<>
bool deserializeBasicTypes<bool>(std::istream& istream) {
	return deserializeBasicTypes<char>(istream) != 0;
}

void serializeString(const std::string& str, std::ostream& ostream) {
	serializeBasicTypes(str.c_str(), str.length(), ostream);
	serializeBasicTypes<char>('\0', ostream);
}

std::string deserializeString(std::istream& istream) {
	std::stringstream sstream;

	while(char c = istream.get()) {
		sstream << c;
	}

	return sstream.str();
}
};