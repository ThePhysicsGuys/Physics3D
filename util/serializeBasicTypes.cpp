#include "serializeBasicTypes.h"


void serialize(const char* data, size_t size, std::ostream& ostream) {
	ostream.write(data, size);
}

void deserialize(char* buf, size_t size, std::istream& istream) {
	for(size_t i = 0; i < size; i++)
		buf[i] = istream.get();
}

template<>
void serialize<char>(const char& c, std::ostream& ostream) {
	ostream << c;
}
template<>
char deserialize<char>(std::istream& istream) {
	return istream.get();
}

