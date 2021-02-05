#pragma once

#include "mat.h"
#include <utility>
#include <assert.h>

template<typename T>
class UnmanagedLargeVector {
public:
	T* data;
	size_t n;

	UnmanagedLargeVector() : n(0), data(nullptr) {}
	UnmanagedLargeVector(T* dataBuf, size_t size) : data(dataBuf), n(size) {}

	UnmanagedLargeVector(UnmanagedLargeVector&& other) noexcept : data(other.data), n(other.n) {
		other.data = nullptr;
		other.n = 0;
	}

	inline UnmanagedLargeVector& operator=(UnmanagedLargeVector&& other) noexcept {
		std::swap(this->data, other.data);
		std::swap(this->n, other.n);
		return *this;
	}

	size_t size() const { return n; }

	template<typename VectorType>
	inline void setSubVector(const VectorType& vec, size_t offset = 0) {
		assert(offset + vec.size() <= n);
		for(size_t i = 0; i < vec.size(); i++) {
			this->data[i + offset] = vec[i];
		}
	}

	template<size_t Size>
	inline Vector<T, Size> getSubVector(size_t offset = 0) {
		assert(offset + Size <= n);
		Vector<T, Size> result;
		for (size_t i = 0; i < Size; i++) {
			result[i] = this->data[i + offset];
		}
		return result;
	}

	inline UnmanagedLargeVector<T> subVector(size_t offset, size_t size) {
		assert(offset + size <= n);
		return UnmanagedLargeVector<T>(this->data + offset, size);
	}
	inline UnmanagedLargeVector<const T> subVector(size_t offset, size_t size) const {
		assert(offset + size <= n);
		return UnmanagedLargeVector<const T>(this->data + offset, size);
	}

	T& operator[] (size_t index) {
		assert(index >= 0 && index < n);
		return data[index];
	}
	const T& operator[] (size_t index) const {
		assert(index >= 0 && index < n);
		return data[index];
	}

	inline UnmanagedLargeVector& operator+=(const UnmanagedLargeVector& other) {
		assert(this->n == other.n);

		for(size_t i = 0; i < n; i++) {
			this->data[i] += other.data[i];
		}
		return *this;
	}
	inline UnmanagedLargeVector& operator-=(const UnmanagedLargeVector& other) {
		assert(this->n == other.n);

		for(size_t i = 0; i < n; i++) {
			this->data[i] -= other.data[i];
		}
		return *this;
	}
};

template<typename T>
class LargeVector : public UnmanagedLargeVector<T> {
public:
	LargeVector() = default;
	LargeVector(size_t size) : UnmanagedLargeVector<T>(new T[size], size) {}

	LargeVector(size_t size, const T* initialData) : UnmanagedLargeVector<T>(new T[size], size) {
		for(size_t i = 0; i < size; i++) {
			this->data[i] = initialData[i];
		}
	}
	~LargeVector() {
		delete[] this->data;
	}

	LargeVector(const LargeVector& other) : UnmanagedLargeVector<T>(new T[other.n], other.n) {
		for(int i = 0; i < other.n; i++) {
			this->data[i] = other.data[i];
		}
	}
	inline LargeVector& operator=(const LargeVector& other) {
		delete[] this->data;
		this->n = other.n;
		this->data = new T[other.n];
		for(int i = 0; i < other.n; i++) {
			this->data[i] = other.data[i];
		}
		return *this;
	}
};

template<typename T>
class UnmanagedLargeMatrix {
public:
	T* data;
	size_t w, h;

	UnmanagedLargeMatrix() : w(0), h(0), data(nullptr) {}
	UnmanagedLargeMatrix(T* buffer, size_t width, size_t height) : w(width), h(height), data(buffer) {}

	inline UnmanagedLargeMatrix(UnmanagedLargeMatrix&& other) noexcept : data(other.data), w(other.w), h(other.h) {
		other.data = nullptr;
		other.w = 0;
		other.h = 0;
	}

	inline UnmanagedLargeMatrix& operator=(UnmanagedLargeMatrix&& other) noexcept {
		std::swap(this->data, other.data);
		std::swap(this->w, other.w);
		std::swap(this->h, other.h);
		return *this;
	}

	T& operator()(size_t row, size_t col) {
		assert(row >= 0 && row < h);
		assert(col >= 0 && col < w);
		return data[w * row + col];
	}

	const T& operator()(size_t row, size_t col) const {
		assert(row >= 0 && row < h);
		assert(col >= 0 && col < w);
		return data[w * row + col];
	}

	void setSubMatrix(size_t topLeftRow, size_t topLeftCol, const UnmanagedLargeMatrix& matrix) {
		for (size_t row = 0; row < matrix.h; row++) {
			for (size_t col = 0; col < matrix.w; col++) {
				(*this)(row + topLeftRow, col + topLeftCol) = matrix(row, col);
			}
		}
	}

	size_t width() const { return w; }
	size_t height() const { return h; }

	LargeVector<T> getRow(size_t row) const {
		LargeVector<T> result(w);
		for(size_t i = 0; i < w; i++) {
			result[i] = (*this)(row, i);
		}
		return result;
	}
	LargeVector<T> getCol(size_t col) const {
		LargeVector<T> result(h);
		for(size_t i = 0; i < h; i++) {
			result[i] = (*this)(i, col);
		}
		return result;
	}
	template<typename VectorType>
	void setRow(size_t row, const VectorType& value) {
		assert(w == value.size());
		for(size_t i = 0; i < w; i++) {
			(*this)(row, i) = value[i];
		}
	}
	template<typename VectorType>
	void setCol(size_t col, const VectorType& value) {
		assert(h == value.size());
		for(size_t i = 0; i < h; i++) {
			(*this)(i, col) = value[i];
		}
	}
	
	inline UnmanagedLargeMatrix& operator+=(const UnmanagedLargeMatrix& other) {
		assert(this->w == other.w);
		assert(this->h == other.h);

		for(size_t i = 0; i < w * h; i++) {
			this->data[i] += other.data[i];
		}
		return *this;
	}

	inline UnmanagedLargeMatrix& operator-=(const UnmanagedLargeMatrix& other) {
		assert(this->w == other.w);
		assert(this->h == other.h);

		for(size_t i = 0; i < w * h; i++) {
			this->data[i] -= other.data[i];
		}
		return *this;
	}

	T* begin() {return data;}
	T* end() {return data + w * h;}
	const T* begin() const { return data; }
	const T* end() const { return data + w * h; }
};

template<typename T>
class LargeMatrix : public UnmanagedLargeMatrix<T> {
public:
	LargeMatrix() = default;
	LargeMatrix(size_t width, size_t height) : UnmanagedLargeMatrix<T>(new T[width * height], width, height) {}
	~LargeMatrix() {
		delete[] this->data;
	}
	LargeMatrix(const LargeMatrix& other) : UnmanagedLargeMatrix<T>(new T[other.w * other.h], other.w, other.h) {
		for(size_t i = 0; i < other.w * other.h; i++) {
			this->data[i] = other.data[i];
		}
	}
	LargeMatrix& operator=(const LargeMatrix& other) {
		delete[] this->data;
		this->h = other.h;
		this->w = other.w;
		this->data = new T[other.h * other.w];
		for(size_t i = 0; i < other.w * other.h; i++) {
			this->data[i] = other.data[i];
		}
		return *this;
	}
	LargeMatrix(LargeMatrix&& other) noexcept : UnmanagedLargeMatrix<T>(other.data, other.w, other.h) {
		other.data = nullptr;
		other.w = 0;
		other.h = 0;
	}
	LargeMatrix& operator=(LargeMatrix&& other) noexcept {
		std::swap(this->data, other.data);
		std::swap(this->w, other.w);
		std::swap(this->h, other.h);
		return *this;
	}

	static LargeMatrix zero(size_t width, size_t height) {
		LargeMatrix result(width, height);
		for(size_t i = 0; i < width * height; i++) {
			result.data[i] = 0;
		}
		return result;
	}
};

constexpr size_t getAmountOfElementsForSymmetric(size_t size) {
	return size * (size + 1) / 2;
}

template<typename T>
class UnmanagedLargeSymmetricMatrix {
public:
	T* data;
	size_t size;

	UnmanagedLargeSymmetricMatrix() : data(nullptr), size(0) {}
	UnmanagedLargeSymmetricMatrix(T* data, size_t size) : data(data), size(size) {}

	inline UnmanagedLargeSymmetricMatrix(UnmanagedLargeSymmetricMatrix&& other) noexcept : data(other.data), size(other.size) {
		other.data = nullptr;
		other.size = 0;
	}

	inline UnmanagedLargeSymmetricMatrix& operator=(UnmanagedLargeSymmetricMatrix&& other) noexcept {
		std::swap(this->data, other.data);
		std::swap(this->size, other.size);
		return *this;
	}

	size_t width() const { return size; }
	size_t height() const { return size; }

	LargeVector<T> getRow(size_t row) const {
		LargeVector<T> result(size);
		for(size_t i = 0; i < size; i++) {
			result[i] = (*this)(row, i);
		}
		return result;
	}
	LargeVector<T> getCol(size_t col) const {
		LargeVector<T> result(size);
		for(size_t i = 0; i < size; i++) {
			result[i] = (*this)(i, col);
		}
		return result;
	}
	template<typename VectorType>
	void setRow(size_t row, const VectorType& value) {
		assert(size == value.size());
		for(size_t i = 0; i < size; i++) {
			(*this)(row, i) = value[i];
		}
	}
	template<typename VectorType>
	void setCol(size_t col, const VectorType& value) {
		assert(size == value.size());
		for(size_t i = 0; i < size; i++) {
			(*this)(i, col) = value[i];
		}
	}
	
	inline UnmanagedLargeSymmetricMatrix& operator+=(const UnmanagedLargeSymmetricMatrix& other) {
		assert(this->size == other.size);

		for(size_t i = 0; i < size * (size + 1) / 2; i++) {
			this->data[i] += other.data[i];
		}
		return *this;
	}
	inline UnmanagedLargeSymmetricMatrix& operator-=(const UnmanagedLargeSymmetricMatrix& other) {
		assert(this->size == other.size);

		for(size_t i = 0; i < size * (size + 1) / 2; i++) {
			this->data[i] -= other.data[i];
		}
		return *this;
	}

	T& operator()(size_t row, size_t col) {
		size_t a = (row >= col) ? row : col; // max
		size_t b = (row >= col) ? col : row; // min

		assert(b >= 0);
		assert(a < size);

		return this->data[a * (a + 1) / 2 + b];
	}
	const T& operator()(size_t row, size_t col) const {
		size_t a = (row >= col) ? row : col; // max
		size_t b = (row >= col) ? col : row; // min

		assert(b >= 0);
		assert(a < size);

		return this->data[a * (a + 1) / 2 + b];
	}
};

template<typename T>
class LargeSymmetricMatrix : public UnmanagedLargeSymmetricMatrix<T> {
public:
	LargeSymmetricMatrix() = default;
	LargeSymmetricMatrix(size_t size) : UnmanagedLargeSymmetricMatrix<T>(new T[getAmountOfElementsForSymmetric(size)], size) {}
	~LargeSymmetricMatrix() { delete[] this->data; }

	LargeSymmetricMatrix(const LargeSymmetricMatrix& other) : UnmanagedLargeSymmetricMatrix<T>(new T[getAmountOfElementsForSymmetric(other.size)], other.size) {
		for(size_t i = 0; i < getAmountOfElementsForSymmetric(other.size); i++) {
			this->data[i] = other.data[i];
		}
	}
	inline LargeSymmetricMatrix& operator=(const LargeSymmetricMatrix& other) {
		delete[] this->data;
		this->size = other.size;
		this->data = new T[getAmountOfElementsForSymmetric(other.size)];
		for(size_t i = 0; i < getAmountOfElementsForSymmetric(other.size); i++) {
			this->data[i] = other.data[i];
		}
		return *this;
	}
};

template<typename T, size_t Rows>
class UnmanagedVerticalFixedMatrix {
public:
	T* data;
	size_t cols;

	UnmanagedVerticalFixedMatrix() : data(nullptr), cols(0) {}
	UnmanagedVerticalFixedMatrix(T* data, size_t width) : data(data), cols(width) {}

	inline UnmanagedVerticalFixedMatrix(UnmanagedVerticalFixedMatrix&& other) : data(other.data), cols(other.cols) {
		other.data = nullptr;
		other.cols = 0;
	}

	inline UnmanagedVerticalFixedMatrix& operator=(UnmanagedVerticalFixedMatrix&& other) {
		std::swap(this->data, other.data);
		std::swap(this->cols, other.cols);
		return *this;
	}

	size_t width() const { return cols; }
	constexpr size_t height() const { return Rows; }
	
	LargeVector<T> getRow(size_t row) const {
		LargeVector<T> result(cols);
		for(size_t i = 0; i < cols; i++) {
			result[i] = (*this)(row, i);
		}
		return result;
	}
	Vector<T, Rows> getCol(size_t col) const {
		Vector<T, Rows> result;
		for(size_t i = 0; i < Rows; i++) {
			result[i] = (*this)(i, col);
		}
		return result;
	}
	template<typename VectorType>
	void setRow(size_t row, const VectorType& value) {
		assert(cols == value.size());
		for(size_t i = 0; i < cols; i++) {
			(*this)(row, i) = value[i];
		}
	}
	void setCol(size_t col, const Vector<T, Rows>& value) {
		for(size_t i = 0; i < Rows; i++) {
			(*this)(i, col) = value[i];
		}
	}

	UnmanagedVerticalFixedMatrix<T, Rows> subCols(size_t offset, size_t size) {
		assert(offset + size <= cols);
		return UnmanagedVerticalFixedMatrix<T, Rows>(data + Rows * offset, size);
	}
	UnmanagedVerticalFixedMatrix<const T, Rows> subCols(size_t offset, size_t size) const {
		assert(offset + size <= cols);
		return UnmanagedVerticalFixedMatrix<const T, Rows>(data + Rows * offset, size);
	}

	inline UnmanagedVerticalFixedMatrix& operator+=(const UnmanagedVerticalFixedMatrix& other) {
		assert(this->cols == other.cols);

		for(size_t i = 0; i < Rows * cols; i++) {
			this->data[i] += other.data[i];
		}
		return *this;
	}
	inline UnmanagedVerticalFixedMatrix& operator-=(const UnmanagedVerticalFixedMatrix& other) {
		assert(this->cols == other.cols);

		for(size_t i = 0; i < Rows * cols; i++) {
			this->data[i] -= other.data[i];
		}
		return *this;
	}

	T& operator()(size_t row, size_t col) {
		assert(row >= 0 && row < Rows && col >= 0 && col < cols);
		return data[col * Rows + row];
	}
	const T& operator()(size_t row, size_t col) const {
		assert(row >= 0 && row < Rows && col >= 0 && col < cols);
		return data[col * Rows + row];
	}
	T* begin() { return data; }
	const T* begin() const { return data; }
	T* end() { return data + cols*Rows; }
	const T* end() const { return data + cols*Rows; }
};

template<typename T, size_t Rows>
class VerticalFixedMatrix : public UnmanagedVerticalFixedMatrix<T, Rows> {
public:
	VerticalFixedMatrix() = default;
	VerticalFixedMatrix(size_t cols) : UnmanagedVerticalFixedMatrix<T, Rows>(new T[cols * Rows], cols * Rows) {}
	~VerticalFixedMatrix() { delete[] this->data; }

	VerticalFixedMatrix(const VerticalFixedMatrix& other) : UnmanagedVerticalFixedMatrix<T, Rows>(new T[other.cols * Rows], other.cols * Rows) {
		for(size_t i = 0; i < other.cols * Rows; i++) {
			this->data[i] = other.data[i];
		}
	}
	inline VerticalFixedMatrix& operator=(const VerticalFixedMatrix& other) {
		delete[] this->data;
		this->size = other.size;
		this->data = new T[other.cols * Rows];
		for(size_t i = 0; i < other.cols * Rows; i++) {
			this->data[i] = other.data[i];
		}
		return *this;
	}
};

template<typename T, size_t Cols>
class UnmanagedHorizontalFixedMatrix {
public:
	T* data;
	size_t rows;

	UnmanagedHorizontalFixedMatrix() : data(nullptr), rows(0) {}
	UnmanagedHorizontalFixedMatrix(T* data, size_t width) : data(data), rows(width) {}

	inline UnmanagedHorizontalFixedMatrix(UnmanagedHorizontalFixedMatrix&& other) : data(other.data), rows(other.rows) {
		other.data = nullptr;
		other.rows = 0;
	}

	inline UnmanagedHorizontalFixedMatrix& operator=(UnmanagedHorizontalFixedMatrix&& other) {
		std::swap(this->data, other.data);
		std::swap(this->rows, other.rows);
		return *this;
	}

	constexpr size_t width() const { return Cols; }
	size_t height() const { return rows; }
	
	Vector<T, Cols> getRow(size_t row) const {
		Vector<T, Cols> result;
		for(size_t i = 0; i < Cols; i++) {
			result[i] = (*this)(row, i);
		}
		return result;
	}
	LargeVector<T> getCol(size_t col) const {
		LargeVector<T> result(rows);
		for(size_t i = 0; i < rows; i++) {
			result[i] = (*this)(i, col);
		}
		return result;
	}
	void setRow(size_t row, const Vector<T, Cols>& value) {
		for(size_t i = 0; i < Cols; i++) {
			(*this)(row, i) = value[i];
		}
	}
	template<typename VectorType>
	void setCol(size_t col, const VectorType& value) {
		assert(rows == value.size());
		for(size_t i = 0; i < rows; i++) {
			(*this)(i, col) = value[i];
		}
	}

	UnmanagedHorizontalFixedMatrix<T, Cols> subRows(size_t offset, size_t size) {
		assert(offset + size <= rows);
		return UnmanagedHorizontalFixedMatrix<T, Cols>(data + Cols * offset, size);
	}
	UnmanagedHorizontalFixedMatrix<const T, Cols> subRows(size_t offset, size_t size) const {
		assert(offset + size <= rows);
		return UnmanagedHorizontalFixedMatrix<const T, Cols>(data + Cols * offset, size);
	}

	inline UnmanagedHorizontalFixedMatrix& operator+=(const UnmanagedHorizontalFixedMatrix& other) {
		assert(this->rows == other.rows);

		for(size_t i = 0; i < rows * Cols; i++) {
			this->data[i] += other.data[i];
		}
		return *this;
	}
	inline UnmanagedHorizontalFixedMatrix& operator-=(const UnmanagedHorizontalFixedMatrix& other) {
		assert(this->rows == other.rows);

		for(size_t i = 0; i < rows * Cols; i++) {
			this->data[i] -= other.data[i];
		}
		return *this;
	}

	T& operator()(size_t row, size_t col) {
		assert(row >= 0 && row < rows && col >= 0 && col < Cols);
		return data[row * Cols + col];
	}
	const T& operator()(size_t row, size_t col) const {
		assert(row >= 0 && row < rows && col >= 0 && col < Cols);
		return data[row * Cols + col];
	}
	T* begin() { return data; }
	const T* begin() const { return data; }
	T* end() { return data + rows * Cols; }
	const T* end() const { return data + rows * Cols; }
};

template<typename T, size_t Cols>
class HorizontalFixedMatrix : public UnmanagedHorizontalFixedMatrix<T, Cols> {
public:
	HorizontalFixedMatrix() = default;
	HorizontalFixedMatrix(size_t cols) : UnmanagedHorizontalFixedMatrix<T, Cols>(new T[cols * Cols], cols) {}
	~HorizontalFixedMatrix() { delete[] this->data; }

	HorizontalFixedMatrix(const HorizontalFixedMatrix& other) : UnmanagedHorizontalFixedMatrix<T, Cols>(new T[other.rows * Cols], other.rows * Cols) {
		for(size_t i = 0; i < other.rows * Cols; i++) {
			this->data[i] = other.data[i];
		}
	}
	inline HorizontalFixedMatrix& operator=(const HorizontalFixedMatrix& other) {
		delete[] this->data;
		this->size = other.size;
		this->data = new T[other.rows * Cols];
		for(size_t i = 0; i < other.rows * Cols; i++) {
			this->data[i] = other.data[i];
		}
		return *this;
	}
};

/*
	Operators
*/

template<typename T>
LargeVector<T> operator*(const UnmanagedLargeMatrix<T>& m, const UnmanagedLargeVector<T>& v) {
	assert(v.n == m.w);
	LargeVector<T> newVector(m.h);

	for(size_t i = 0; i < m.h; i++) {
		T total = m(i, 0) * v[0];

		for(size_t j = 1; j < m.w; j++) {
			total += m(i, j) * v[j];
		}
		newVector[i] = total;
	}
	return newVector;
}

template<typename T>
LargeVector<T> operator*(const UnmanagedLargeSymmetricMatrix<T>& m, const UnmanagedLargeVector<T>& v) {
	assert(v.n == m.size);
	LargeVector<T> newVector(m.size);

	for(size_t i = 0; i < m.size; i++) {
		T total = m(i, 0) * v[0];

		for(size_t j = 1; j < m.size; j++) {
			total += m(i, j) * v[j];
		}
		newVector[i] = total;
	}
	return newVector;
}

template<typename T, size_t Size>
Vector<T, Size> operator*(const UnmanagedVerticalFixedMatrix<T, Size>& m, const UnmanagedLargeVector<T>& v) {
	assert(m.cols == v.n);
	Vector<T, Size> result;

	for(size_t i = 0; i < Size; i++) {
		T total = m(i, 0) * v[0];

		for(size_t j = 1; j < m.cols; j++) {
			total += m(i, j) * v[j];
		}
		result[i] = total;
	}
	return result;
}

template<typename T, size_t Height, size_t Width>
Matrix<T, Height, Width> operator*(const UnmanagedVerticalFixedMatrix<T, Height>& m1, const UnmanagedHorizontalFixedMatrix<T, Width>& m2) {
	assert(m1.cols == m2.rows);
	Matrix<T, Height, Width> result;

	inMemoryMatrixMultiply(m1, m2, result);
	return result;
}

template<typename M1, typename M2, typename MResult>
void inMemoryMatrixMultiply(const M1& m1, const M2& m2, MResult& result) {
	assert(m1.height() == result.height()); // result height
	assert(m2.width() == result.width()); // result width
	assert(m1.width() == m2.height()); // intermediate

	for(std::size_t col = 0; col < result.width(); col++) {
		for(std::size_t row = 0; row < result.height(); row++) {
			auto sum = m1(row, 0) * m2(0, col);
			for(std::size_t i = 1; i < m1.width(); i++) {
				sum += m1(row, i) * m2(i, col);
			}
			result(row, col) = sum;
		}
	}
}

template<typename M, typename V, typename VResult>
void inMemoryMatrixVectorMultiply(const M& m, const V& v, VResult& result) {
	assert(v.size() == m.width());
	assert(result.size() == m.height());

	for(std::size_t row = 0; row < m.height(); row++) {
		auto sum = m(row, 0) * v[0];
		for(std::size_t col = 1; col < m.width(); col++) {
			sum += m(row, col) * v[col];
		}
		result[row] = sum;
	}
}

template<typename V>
void inMemoryVectorNegate(V& v) {
	for(std::size_t i = 0; i < v.size(); i++) {
		v[i] = -v[i];
	}
}

template<typename M>
void inMemoryMatrixNegate(M& m) {
	for(std::size_t row = 0; row < m.height(); row++) {
		for(std::size_t col = 0; col < m.width(); col++) {
			m(row, col) = -m(row, col);
		}
	}
}
