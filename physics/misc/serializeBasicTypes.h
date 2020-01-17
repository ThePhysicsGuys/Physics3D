#pragma once

#include <iostream>

#include "../math/fix.h"
#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/cframe.h"
#include "../math/globalcframe.h"


void serialize(const char* data, size_t size, std::ostream& ostream);
void deserialize(char* buf, size_t size, std::istream& istream);

/*
	Basic value serialization
	Included are: char, int, float, double, long, Fix, Vector, Matrix, SymmetricMatrix, DiagonalMatrix, CFrame, Transform, GlobalCFrame, GlobalTransform, Bounds, GlobalBounds
*/
template<typename T>
void serialize(const T& i, std::ostream& ostream) {
	serialize(reinterpret_cast<const char*>(&i), sizeof(T), ostream);
}

/*
	Basic value deserialization
	Included are: char, int, float, double, long, Fix, Vector, Matrix, SymmetricMatrix, DiagonalMatrix, CFrame, Transform, GlobalCFrame, GlobalTransform, Bounds, GlobalBounds
*/
template<typename T>
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
