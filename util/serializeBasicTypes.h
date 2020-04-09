#pragma once

#include <iostream>
#include <exception>
#include <string>
#include <type_traits>

class SerializationException : public std::exception {
	std::string message;
public:
	SerializationException() = default;
	SerializationException(std::string message) : message(message) {}

	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};

void serialize(const char* data, size_t size, std::ostream& ostream);
void deserialize(char* buf, size_t size, std::istream& istream);

/*
	Trivial value serialization
	Included are: char, int, float, double, long, Fix, Vector, Matrix, SymmetricMatrix, DiagonalMatrix, CFrame, Transform, GlobalCFrame, GlobalTransform, Bounds, GlobalBounds
*/
template<typename T, std::enable_if_t<std::is_trivially_copyable<T>::value, int> = 0>
void serialize(const T& i, std::ostream& ostream) {
	serialize(reinterpret_cast<const char*>(&i), sizeof(T), ostream);
}

/*
	Trivial value deserialization
	Included are: char, int, float, double, long, Fix, Vector, Matrix, SymmetricMatrix, DiagonalMatrix, CFrame, Transform, GlobalCFrame, GlobalTransform, Bounds, GlobalBounds
*/
template<typename T, std::enable_if_t<std::is_trivially_copyable<T>::value, int> = 0>
T deserialize(std::istream& istream) {
	union {
		char buf[sizeof(T)];
		T value;
	} un{};
	deserialize(un.buf, sizeof(T), istream);
	return un.value;
}

template<>
void serialize<char>(const char& c, std::ostream& ostream);
template<>
char deserialize<char>(std::istream& istream);

void serializeString(const std::string& str, std::ostream& ostream);
std::string deserializeString(std::istream& istream);

template<typename T>
void serializeArray(const T* data, size_t size, std::ostream& ostream) {
	for(size_t i = 0; i < size; i++) {
		serialize<T>(data[i], ostream);
	}
}

template<typename T>
void deserializeArray(T* buf, size_t size, std::istream& istream) {
	for(size_t i = 0; i < size; i++) {
		buf[i] = deserialize<T>(istream);
	}
}

