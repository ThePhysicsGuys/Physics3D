#pragma once

#include <iostream>
#include <fstream>

#include "../math/fix.h"
#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/cframe.h"
#include "../math/globalcframe.h"
#include "../geometry/polyhedron.h"
#include "../geometry/shape.h"

#include "../part.h"
#include "../world.h"
#include "../physical.h"

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

template<>
void serialize<Polyhedron>(const Polyhedron& poly, std::ostream& ostream);
template<>
Polyhedron deserialize<Polyhedron>(std::istream& istream);

template<>
void serialize<Shape>(const Shape& shape, std::ostream& ostream);
template<>
Shape deserialize<Shape>(std::istream& istream);

template<>
void serialize<Part>(const Part& part, std::ostream& ostream);
template<>
Part deserialize<Part>(std::istream& istream);



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


template<typename T>
void save(const T& obj, std::string fileName) {
	std::ofstream file;
	file.open(fileName, std::ios::binary);
	serialize(obj, file);
	file.close();
}

template<typename T>
void saveClassObj(const T& obj, std::string fileName) {
	std::ofstream file;
	file.open(fileName, std::ios::binary);
	obj.serialize(file);
	file.close();
}

template<typename T>
T load(std::string fileName) {
	std::ifstream file;
	file.open(fileName, std::ios::binary);
	T result = deserialize<T>(file);
	file.close();
	return result;
}

template<typename T>
T loadClassObj(std::string fileName) {
	std::ifstream file;
	file.open(fileName, std::ios::binary);
	T result = T::deserialize(file);
	file.close();
	return result;
}