#pragma once

#include "mat.h"
#include <utility>

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
	LargeVector(const LargeVector& other) : size(other.size), data(new T[other.size]) {
		for (int i = 0; i < other.size; i++) {
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
		for (size_t i = 0; i < Size; i++) {
			this->data[i + offset] = vec[i];
		}
	}

	template<template<typename, size_t> typename VectorBasis, size_t Size>
	inline VectorBasis<T, Size> getSubVector(size_t offset) {
		VectorBasis<T, Size> result;
		for (size_t i = 0; i < Size; i++) {
			result[i] = this->data[i + offset];
		}
		return result;
	}

	~LargeVector() { delete[] data; }
	T& operator[] (size_t index) {
		assert(index >= 0 && index < size);
		return data[index];
	}
	const T& operator[] (size_t index) const {
		assert(index >= 0 && index < size);
		return data[index];
	}
};

template<typename T>
class LargeMatrix {
	T* data;
public:
	size_t width, height;

	LargeMatrix(size_t N, size_t M) : width(N), height(M), data(new T[N*M]) {}
	~LargeMatrix() { delete[] data; }

	LargeMatrix(const LargeMatrix& other) : width(other.width), height(other.height), data(new T[other.width * other.height]) {
		for (size_t i = 0; i < other.width * other.height; i++) {
			this->data[i] = other.data[i];
		}
	}
	inline LargeMatrix& operator=(const LargeMatrix& other) {
		delete[] data;
		this->height = other.height;
		this->width = other.width;
		data = new T[height * width];
		for (size_t i = 0; i < width * height; i++) {
			this->data[i] = other.data[i];
		}
		return *this;
	}

	inline LargeMatrix(LargeMatrix&& other) : data(other.data), width(other.width), height(other.height) {
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

	T& get(size_t row, size_t col) {
		assert(row >= 0 && row < height);
		assert(col >= 0 && col < width);
		return data[width * row + col];
	}

	const T& get(size_t row, size_t col) const {
		assert(row >= 0 && row < height);
		assert(col >= 0 && col < width);
		return data[width * row + col];
	}

	template<size_t Width, size_t Height>
	void setSubMatrix(size_t topLeftRow, size_t topLeftCol, const Matrix<T, Width, Height>& matrix) {
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
				this->get(row + topLeftRow, col + topLeftCol) = matrix(row, col);
			}
		}
	}

	void setSubMatrix(size_t topLeftRow, size_t topLeftCol, const LargeMatrix& matrix) {
		for (size_t row = 0; row < matrix.height; row++) {
			for (size_t col = 0; col < matrix.width; col++) {
				this->get(row + topLeftRow, col + topLeftCol) = matrix(row, col);
			}
		}
	}

	T* begin() {return data;}
	T* end() {return data + width * height;}
	const T* begin() const { return data; }
	const T* end() const { return data + width * height; }
};

constexpr size_t getAmountOfElementsForSymmetric(size_t size) {
	return size * (size + 1) / 2;
}

template<typename T>
class LargeSymmetricMatrix {
	T* data;
public:
	size_t size;

	LargeSymmetricMatrix(size_t size) : size(size), data(new T[getAmountOfElementsForSymmetric(size)]) {}
	~LargeSymmetricMatrix() { delete[] data; }

	LargeSymmetricMatrix(const LargeSymmetricMatrix& other) : data(new T[getAmountOfElementsForSymmetric(size)]), size(other.size) {
		for(size_t i = 0; i < getAmountOfElementsForSymmetric(size); i++) {
			this->data[i] = other.data[i];
		}
	}
	inline LargeSymmetricMatrix& operator=(const LargeSymmetricMatrix& other) {
		delete[] data;
		this->size = other.size;
		data = new T[getAmountOfElementsForSymmetric(size)];
		for(size_t i = 0; i < getAmountOfElementsForSymmetric(size); i++) {
			this->data[i] = other.data[i];
		}
		return *this;
	}

	inline LargeSymmetricMatrix<T>(LargeSymmetricMatrix<T>&& other) : data(other.data), size(other.size) {
		other.data = nullptr;
		other.size = 0;
	}

	inline LargeSymmetricMatrix& operator=(LargeSymmetricMatrix&& other) {
		std::swap(this->data, other.data);
		std::swap(this->size, other.size);
		return *this;
	}

	T& get(size_t row, size_t col) {
		size_t a = (row >= col) ? row : col; // max
		size_t b = (row >= col) ? col : row; // min

		assert(b >= 0);
		assert(a < size);

		return this->data[a * (a + 1) / 2 + b];
	}
	const T& get(size_t row, size_t col) const {
		size_t a = (row >= col) ? row : col; // max
		size_t b = (row >= col) ? col : row; // min

		assert(b >= 0);
		assert(a < size);

		return this->data[a * (a + 1) / 2 + b];
	}
};

template<typename T>
LargeVector<T> operator*(const LargeMatrix<T>& m, const LargeVector<T>& v) {
	if (v.size != m.width) throw "Dimensions do not align!";
	LargeVector<T> newVector(m.height);

	for (size_t i = 0; i < m.height; i++) {
		T total = m.get(i, 0) * v[0];

		for (size_t j = 1; j < m.width; j++) {
			total += m.get(i, j) * v[j];
		}
		newVector[i] = total;
	}
	return newVector;
}

template<typename T>
LargeVector<T> operator*(const LargeSymmetricMatrix<T>& m, const LargeVector<T>& v) {
	if(v.size != m.size) throw "Dimensions do not align!";
	LargeVector<T> newVector(m.size);

	for(size_t i = 0; i < m.size; i++) {
		T total = m.get(0, i) * v[0];

		for(size_t j = 1; j < m.size; j++) {
			total += m.get(0, j) * v[j];
		}
		newVector[i] = total;
	}
	return newVector;
}

template<typename T>
void destructiveSolve(LargeMatrix<T>& m, LargeVector<T>& v);


