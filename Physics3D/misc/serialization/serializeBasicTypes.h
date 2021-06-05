#pragma once

#include <iostream>
#include <exception>
#include <string>
#include <type_traits>

namespace P3D {
class SerializationException : public std::exception {
	std::string message;
public:
	SerializationException() = default;
	SerializationException(std::string message) : message(message) {}

	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};

void serializeBasicTypes(const char* data, size_t size, std::ostream& ostream);
void deserializeBasicTypes(char* buf, size_t size, std::istream& istream);

/*
	Trivial value serialization
	Included are: char, int, float, double, long, Fix, Vector, Matrix, SymmetricMatrix, DiagonalMatrix, CFrame, Transform, GlobalCFrame, GlobalTransform, Bounds, GlobalBounds
*/
template<typename T, std::enable_if_t<std::is_trivially_copyable<T>::value, int> = 0>
void serializeBasicTypes(const T& i, std::ostream& ostream) {
	serializeBasicTypes(reinterpret_cast<const char*>(&i), sizeof(T), ostream);
}

/*
	Trivial value deserialization
	Included are: char, int, float, double, long, Fix, Vector, Matrix, SymmetricMatrix, DiagonalMatrix, CFrame, Transform, GlobalCFrame, GlobalTransform, Bounds, GlobalBounds
*/
template<typename T, std::enable_if_t<std::is_trivially_copyable<T>::value, int> = 0>
T deserializeBasicTypes(std::istream& istream) {
	union {
		char buf[sizeof(T)];
		T value;
	} un{};
	deserializeBasicTypes(un.buf, sizeof(T), istream);
	return un.value;
}

template<>
void serializeBasicTypes<char>(const char& c, std::ostream& ostream);
template<>
char deserializeBasicTypes<char>(std::istream& istream);

template<>
void serializeBasicTypes<bool>(const bool& b, std::ostream& ostream);
template<>
bool deserializeBasicTypes<bool>(std::istream& istream);

void serializeString(const std::string& str, std::ostream& ostream);
std::string deserializeString(std::istream& istream);

template<typename T>
void serializeArray(const T* data, size_t size, std::ostream& ostream) {
	for(size_t i = 0; i < size; i++) {
		serializeBasicTypes<T>(data[i], ostream);
	}
}

template<typename T>
void deserializeArray(T* buf, size_t size, std::istream& istream) {
	for(size_t i = 0; i < size; i++) {
		buf[i] = deserializeBasicTypes<T>(istream);
	}
}
};
