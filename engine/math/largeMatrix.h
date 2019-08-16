#pragma once

#include "mat2.h"
#include "mat3.h"
#include "mat4.h"

template<typename T>
class LargeVector {
	T* data;


public:
	size_t size;

	LargeVector(size_t size) : size(size), data(new T[size]) {}
	LargeVector(size_t size, const T* initialData) : size(size), data(new T[size]) {
		for (size_t i = 0; i < size; i++)
			this->data[i] = initialData[i];
	}
	LargeVector(const LargeVector& other) : size(other.size), data(new T[size]) {
		for (int i = 0; i < size; i++) {
			this->data[i] = other.data[i];
		}
	}
	inline LargeVector& operator=(const LargeVector& other) {
		delete[] data;
		this->size = other.size;
		data = new T[size];
		for (int i = 0; i < size; i++) {
			this->data[i] = other.data[i];
		}
		return *this;
	}

	LargeVector(LargeVector&& other) {
		this->data = other.data;
		this->size = other.size;
		other.data = nullptr;
		other.size = 0;
	}

	inline LargeVector& operator=(LargeVector&& other) {
		std::swap(this->data, other.data);
		std::swap(this->size, other.size);
		return *this;
	}

	template<template<typename, size_t> typename VectorBasis, size_t Size>
	inline void setSubVector(size_t offset, const VectorBasis<T, Size>& vec) {
		for (int i = 0; i < Size; i++) {
			this->data[i + offset] = vec[i];
		}
	}

	template<template<typename, size_t> typename VectorBasis, size_t Size>
	inline VectorBasis<T, Size> getSubVector(size_t offset) {
		VectorBasis<T, Size> result;
		for (int i = 0; i < Size; i++) {
			result[i] = this->data[i + offset];
		}
		return result;
	}

	~LargeVector() { delete[] data; }
	T& operator[] (size_t index) { return data[index]; }
	const T& operator[] (size_t index) const { return data[index]; }
};

template<typename T>
class LargeMatrix {
	T* data;
public:
	size_t width, height;

	LargeMatrix(size_t N, size_t M) : width(N), height(M), data(new T[N*M]) {}
	~LargeMatrix() { delete[] data; }

	LargeMatrix(const LargeMatrix& other) : width(other.width), height(other.height), data(new T[other.width * other.height]) {
		for (int i = 0; i < other.width * other.height; i++) {
			this->data[i] = other.data[i];
		}
	}
	inline LargeMatrix& operator=(const LargeMatrix& other) {
		delete[] data;
		this->height = other.height;
		this->width = other.width;
		data = new T[height * width];
		for (int i = 0; i < width * height; i++) {
			this->data[i] = other.data[i];
		}
		return *this;
	}

	inline LargeMatrix(LargeMatrix&& other) {
		this->data = other.data;
		this->width = other.width;
		this->height = other.height;
		other.data = nullptr;
		other.width = 0;
		other.height = 0;
	}

	inline LargeMatrix& operator=(LargeMatrix&& other) {
		std::swap(this->data, other.data);
		std::swap(this->width, other.width);
		std::swap(this->height, other.height);
		return *this;
	}

	T* operator[](size_t index) {
		return data + width * index;
	}
	const T* operator[](size_t index) const {
		return data + width * index;
	}

	void setSubMatrix(size_t topLeftRow, size_t topLeftCol, const Mat3Template<T>& matrix) {
		for (size_t row = 0; row < 3; row++) {
			for (size_t col = 0; col < 3; col++) {
				(*this)[row + topLeftRow][col + topLeftCol] = matrix[row][col];
			}
		}
	}

	void setSubMatrix(size_t topLeftRow, size_t topLeftCol, const LargeMatrix& matrix) {
		for (size_t row = 0; row < matrix.height; row++) {
			for (size_t col = 0; col < matrix.width; col++) {
				(*this)[row + topLeftRow][col + topLeftCol] = matrix[row][col];
			}
		}
	}

	const T* begin() const { return data; }
	const T* end() const { return data + width * height; }
	T* begin() { return data; }
	T* end() { return data + width * height; }
};

template<typename T>
LargeVector<T> operator*(const LargeMatrix<T>& m, const LargeVector<T>& v) {
	if (v.size != m.width) throw "Dimensions do not align!";
	LargeVector<T> newVector(m.height);

	for (size_t i = 0; i < m.height; i++) {
		const T* row = m[i];

		T total = row[0] * v[0];

		for (size_t j = 1; j < m.width; j++) {
			total += row[j] * v[j];
		}
		newVector[i] = total;
	}
	return newVector;
}

template<typename T>
void destructiveSolve(LargeMatrix<T>& m, LargeVector<T>& v);


