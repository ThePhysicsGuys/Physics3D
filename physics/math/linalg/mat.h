#pragma once

#include "vec.h"
#include <initializer_list>
#include <assert.h>
#include <stddef.h>
#include <cstddef>


template<typename T, std::size_t Width, std::size_t Height>
struct MatrixIndex {
	T* data;
	std::size_t row;
	inline constexpr T& operator[](std::size_t col) const {
		assert(row >= 0 && row < Height);
		assert(col >= 0 && col < Width);

		return data[col * Height + row]; 
	}
};
template<typename T, std::size_t Width, std::size_t Height>
class Matrix {
public:
	T data[Width * Height];


	// Returns a row of this matrix
	inline constexpr MatrixIndex<T, Width, Height> operator[](std::size_t row) { return MatrixIndex<T, Width, Height>{ data, row }; }
	// Returns a row of this matrix
	inline constexpr MatrixIndex<const T, Width, Height> operator[](std::size_t col) const { return MatrixIndex<const T, Width, Height>{ data, col }; }

	Matrix() : data{} {}

	/*
		Initialize matrices like so

		Matrix<T, 4, 3>{
			1, 2, 3, 4,
			5, 6, 7, 8,
			9, 10, 11, 12
		};
	*/
	inline constexpr Matrix(std::initializer_list<T> list) : data{} {
		assert(list.size() == Width * Height);
		auto listIter = list.begin();
		for (std::size_t row = 0; row < Height; row++) {
			for (std::size_t col = 0; col < Width; col++) {
				(*this)[row][col] = *listIter;
				++listIter;
			}
		}
	}

	template<typename OtherT>
	inline constexpr Matrix(const Matrix<OtherT, Width, Height>& m) : data{} {
		for(std::size_t row = 0; row < Height; row++) {
			for(std::size_t col = 0; col < Width; col++) {
				(*this)[row][col] = static_cast<T>(m[row][col]);
			}
		}
	}

	template<typename OtherT1, typename OtherT2, typename OtherT3, typename OtherT4>
	Matrix(const Matrix<OtherT1, Width - 1, Height - 1>& topLeftMat, const Vector<OtherT2, Height - 1>& rightCol, const Vector<OtherT3, Width - 1>& bottomRow, const OtherT4& bottomLeftVal) : data{} {
		for (std::size_t row = 0; row < Height - 1; row++) {
			for (std::size_t col = 0; col < Width - 1; col++) {
				(*this)[row][col] = T(topLeftMat[row][col]);
			}
			(*this)[row][Width - 1] = T(rightCol[row]);
		}
		for (std::size_t col = 0; col < Width - 1; col++) {
			(*this)[Height - 1][col] = T(bottomRow[col]);
		}
		(*this)[Height - 1][Width - 1] = T(bottomLeftVal);
	}

	template<typename OtherT1, typename OtherT2>
	Matrix(const Matrix<OtherT1, Width - 1, Height - 1> & topLeftMat, const OtherT2& bottomLeftVal) : data{} {
		for (std::size_t row = 0; row < Height - 1; row++) {
			for (std::size_t col = 0; col < Width - 1; col++) {
				(*this)[row][col] = topLeftMat[row][col];
			}
			(*this)[row][Width - 1] = 0.0;
		}
		for (std::size_t col = 0; col < Width - 1; col++) {
			(*this)[Height - 1][col] = 0.0;
		}
		(*this)[Height - 1][Width - 1] = bottomLeftVal;
	}
	
	inline static Matrix<T, Width, Height> fromRows(std::initializer_list<Vector<T, Width>> rows) {
		assert(rows.size() == Height);
		Matrix<T, Width, Height> result;
		auto rowIter = rows.begin();
		for(std::size_t row = 0; row < Height; row++) {
			const Vector<T, Width>& curRow = *rowIter;
			for(std::size_t col = 0; col < Width; col++) {
				result[row][col] = curRow[col];
			}
			rowIter++;
		}
		return result;
	}

	inline static Matrix<T, Width, Height> fromColumns(std::initializer_list<Vector<T, Height>> columns) {
		assert(columns.size() == Width);
		Matrix<T, Width, Height> result;
		auto colIter = columns.begin();
		for(std::size_t col = 0; col < Width; col++) {
			const Vector<T, Height>& curCol = *colIter;
			for(std::size_t row = 0; row < Height; row++) {
				result[row][col] = curCol[row];
			}
			colIter++;
		}
		return result;
	}

	void setDataRowMajor(const T* data) {
		for (std::size_t row = 0; row < Height; row++) {
			for (std::size_t col = 0; col < Width; col++) {
				(*this)[row][col] = data[row * Width + col];
			}
		}
	}
	void setDataColMajor(const T* data) {
		for (std::size_t row = 0; row < Height; row++) {
			for (std::size_t col = 0; col < Width; col++) {
				(*this)[row][col] = data[row + col * Height];
			}
		}
	}

	Matrix<T, Height, Width> transpose() const {
		Matrix<T, Height, Width> result;
		for (std::size_t row = 0; row < Height; row++) {
			for (std::size_t col = 0; col < Width; col++) {
				result[col][row] = (*this)[row][col];
			}
		}
		return result;
	}

	template<std::size_t SubWidth, std::size_t SubHeight>
	Matrix<T, SubWidth, SubHeight> getSubMatrix(std::size_t topLeftRow, std::size_t topLeftCol) const {
		assert(topLeftRow >= 0 && topLeftRow + SubHeight <= Height);
		assert(topLeftCol >= 0 && topLeftCol + SubWidth <= Width);

		Matrix<T, SubWidth, SubHeight> result;

		for (std::size_t row = 0; row < SubHeight; row++) {
			for (std::size_t col = 0; col < SubWidth; col++) {
				result[row][col] = (*this)[row+topLeftRow][col+topLeftCol];
			}
		}
		return result;
	}

	template<std::size_t SubWidth, std::size_t SubHeight>
	void setSubMatrix(const Matrix<T, SubWidth, SubHeight>& mat, std::size_t topLeftRow, std::size_t topLeftCol) {
		assert(topLeftRow >= 0 && topLeftRow + SubHeight <= Height);
		assert(topLeftCol >= 0 && topLeftCol + SubWidth <= Width);

		for (std::size_t row = 0; row < SubHeight; row++) {
			for (std::size_t col = 0; col < SubWidth; col++) {
				(*this)[row + topLeftRow][col + topLeftCol] = mat[row][col];
			}
		}
	}

	Matrix<T, Width - 1, Height> withoutCol(std::size_t colToDelete) const {
		assert(colToDelete >= 0 && colToDelete < Width);

		Matrix<T, Width - 1, Height> newMat;
		for (std::size_t row = 0; row < Height; row++) {
			for (std::size_t col = 0; col < colToDelete; col++) {
				newMat[row][col] = (*this)[row][col];
			}
			for (std::size_t col = colToDelete+1; col < Width; col++) {
				newMat[row][col - 1] = (*this)[row][col];
			}
		}
		return newMat;
	}
	Matrix<T, Width, Height - 1> withoutRow(std::size_t rowToDelete) const {
		assert(rowToDelete >= 0 && rowToDelete < Height);

		Matrix<T, Width, Height - 1> newMat;
		for (std::size_t col = 0; col < Width; col++) {
			for (std::size_t row = 0; row < rowToDelete; row++) {
				newMat[row][col] = (*this)[row][col];
			}
			for (std::size_t row = rowToDelete; row < Height - 1; row++) {
				newMat[row][col] = (*this)[row + 1][col];
			}
		}

		return newMat;
	}
	Matrix<T, Width - 1, Height - 1> withoutRowCol(std::size_t rowToDelete, std::size_t colToDelete) const {
		assert(colToDelete >= 0 && colToDelete < Width);
		assert(rowToDelete >= 0 && rowToDelete < Height);

		Matrix<T, Width - 1, Height - 1> newMat;
		for (std::size_t row = 0; row < rowToDelete; row++) {
			for (std::size_t col = 0; col < colToDelete; col++) {
				newMat[row][col] = (*this)[row][col];
			}
			for (std::size_t col = colToDelete + 1; col < Width; col++) {
				newMat[row][col - 1] = (*this)[row][col];
			}
		}
		for (std::size_t row = rowToDelete + 1; row < Height; row++) {
			for (std::size_t col = 0; col < colToDelete; col++) {
				newMat[row - 1][col] = (*this)[row][col];
			}
			for (std::size_t col = colToDelete + 1; col < Width; col++) {
				newMat[row - 1][col - 1] = (*this)[row][col];
			}
		}
		return newMat;
	}

	Vector<T, Width> getRow(std::size_t row) const {
		assert(row >= 0 && row < Height);
		Vector<T, Width> result;

		for(std::size_t i = 0; i < Width; i++) {
			result[i] = (*this)[row][i];
		}
		return result;
	}

	Vector<T, Height> getCol(std::size_t col) const {
		assert(col >= 0 && col < Width);
		Vector<T, Height> result;

		for(std::size_t i = 0; i < Height; i++) {
			result[i] = (*this)[i][col];
		}
		return result;
	}
	
	void setRow(std::size_t row, const Vector<T, Width>& data) {
		assert(row >= 0 && row < Height);

		for (std::size_t i = 0; i < Width; i++) {
			(*this)[row][i] = data[i];
		}
	}
	void setCol(std::size_t col, const Vector<T, Height>& data) {
		assert(col >= 0 && col < Width);

		for (std::size_t i = 0; i < Height; i++) {
			(*this)[i][col] = data[i];
		}
	}

	static inline constexpr Matrix<T, Width, Height> ZEROS() {
		Matrix<T, Width, Height> mat;
		for (std::size_t row = 0; row < Height; row++) {
			for (std::size_t col = 0; col < Width; col++) {
				mat[row][col] = 0;
			}
		}
		return mat;
	}

	static inline constexpr Matrix<T, Width, Height> IDENTITY() {
		Matrix<T, Width, Height> mat;
		for (std::size_t row = 0; row < Height; row++) {
			for (std::size_t col = 0; col < Width; col++) {
				mat[row][col] = (row == col) ? T(1) : T(0);
			}
		}
		return mat;
	}


	static Matrix<T, Width, Height> fromColMajorData(const T* data) {
		Matrix<T, Width, Height> mat;
		for (std::size_t row = 0; row < Height; row++) {
			for (std::size_t col = 0; col < Width; col++) {
				mat[row][col] = data[row + col * Height];
			}
		}
		return mat;
	}

	static Matrix<T, Width, Height> fromRowMajorData(const T* data) {
		Matrix<T, Width, Height> mat;
		for (std::size_t row = 0; row < Height; row++) {
			for (std::size_t col = 0; col < Width; col++) {
				mat[row][col] = data[row * Width + col];
			}
		}
		return mat;
	}

	void toColMajorData(T* buf) const {
		for (std::size_t row = 0; row < Height; row++) {
			for (std::size_t col = 0; col < Width; col++) {
				buf[row + col * Height] = (*this)[row][col];
			}
		}
	}

	void toRowMajorData(T* buf) const {
		for (std::size_t row = 0; row < Height; row++) {
			for (std::size_t col = 0; col < Width; col++) {
				buf[row * Width + col] = (*this)[row][col];
			}
		}
	}
};


/*
	Symmetric matrix indexing
	0
	1 2
	3 4 5
	6 7 8 9
	A B C D E

	[3][0] = 6
	[1][0] = 1
	[2][2] = 5
	[a][b] = a*(a+1)/2+b
*/


template<typename T, std::size_t Size>
struct SymmetricMatrixIndex {
	T* data;
	std::size_t row;

	inline constexpr T& operator[](std::size_t col) const {
		std::size_t a = (row >= col) ? row : col; // max
		std::size_t b = (row >= col) ? col : row; // min

		assert(b >= 0);
		assert(a < Size);

		return data[a * (a + 1) / 2 + b];
	}
};
template<typename T, std::size_t Size>
class SymmetricMatrix {
	T data[Size * (Size + 1) / 2];
public:
	// Returns a row of this matrix
	inline constexpr SymmetricMatrixIndex<T, Size> operator[](std::size_t row) { return SymmetricMatrixIndex<T, Size>{ data, row }; }
	// Returns a row of this matrix
	inline constexpr SymmetricMatrixIndex<const T, Size> operator[](std::size_t row) const { return SymmetricMatrixIndex<const T, Size>{ data, row }; }

	SymmetricMatrix() : data{} {}
	/*
		Initialize symmetric matrices like so

		SymmetricMatrix<T, 4>{
			1,
			2, 3,
			4, 5, 6,
			7, 8, 9, 10
		};
	*/
	inline constexpr SymmetricMatrix(const std::initializer_list<T>& list) : data{} {
		assert(list.size() == Size * (Size + 1) / 2);

		auto listIter = list.begin();
		for (std::size_t row = 0; row < Size; row++) {
			for (std::size_t col = 0; col <= row; col++) {
				(*this)[row][col] = *listIter;
				++listIter;
			}
		}
	}

	template<typename OtherT>
	inline constexpr SymmetricMatrix(const SymmetricMatrix<OtherT, Size>& m) : data{} {
		for(std::size_t row = 0; row < Size; row++) {
			for(std::size_t col = 0; col <= row; col++) {
				(*this)[row][col] = m[row][col];
			}
		}
	}

	static inline constexpr SymmetricMatrix<T, Size> ZEROS() {
		SymmetricMatrix<T, Size> mat;
		for (std::size_t row = 0; row < Size; row++) {
			for (std::size_t col = 0; col <= row; col++) {
				mat[row][col] = 0;
			}
		}
		return mat;
	}

	static inline constexpr SymmetricMatrix<T, Size> IDENTITY() {
		SymmetricMatrix<T, Size> mat;
		for (std::size_t row = 0; row < Size; row++) {
			for (std::size_t col = 0; col <= row; col++) {
				mat[row][col] = (row == col) ? 1 : 0;
			}
		}
		return mat;
	}

	operator Matrix<T, Size, Size>() const {
		Matrix<T, Size, Size> mat;
		for (std::size_t row = 0; row < Size; row++) {
			for (std::size_t col = 0; col <= row; col++) {
				const T& val = (*this)[row][col];
				mat[row][col] = val;
				mat[col][row] = val;
			}
		}
		return mat;
	}

	Vector<T, Size> getRow(std::size_t row) const {
		assert(row >= 0 && row < Size);
		Vector<T, Size> result;

		for(std::size_t i = 0; i < Size; i++) {
			result[i] = (*this)[row][i];
		}
		return result;
	}

	Vector<T, Size> getCol(std::size_t col) const {
		assert(col >= 0 && col < Size);
		Vector<T, Size> result;

		for(std::size_t i = 0; i < Size; i++) {
			result[i] = (*this)[i][col];
		}
		return result;
	}
};

template<typename T, std::size_t Size>
class DiagonalMatrix {
	T data[Size];
public:
	inline constexpr T& operator[](std::size_t index) { assert(index >= 0 && index < Size); return data[index]; }
	inline constexpr const T& operator[](std::size_t index) const { assert(index >= 0 && index < Size); return data[index]; }

	DiagonalMatrix() : data{} {
		for(std::size_t i = 0; i < Size; i++) {
			data[i] = T();
		}
	}
	/*
		Initialize diagonal matrices like so

		DiagonalMatrix<T, 4>{
			1,
			   2,
			      3,
			         4
		};
	*/
	inline constexpr DiagonalMatrix(const std::initializer_list<T>& list) : data{} {
		assert(list.size() == Size);

		auto listIter = list.begin();
		for (std::size_t i = 0; i < Size; i++) {
			(*this)[i] = *listIter;
			++listIter;
		}
	}

	inline constexpr DiagonalMatrix(const T* list) : data{} {
		for(std::size_t i = 0; i < Size; i++) {
			(*this)[i] = list[i];
		}
	}

	template<typename OtherT>
	inline constexpr DiagonalMatrix(const DiagonalMatrix<OtherT, Size>& m) : data{} {
		for(std::size_t i = 0; i < Size; i++) {
			(*this)[i] = static_cast<T>(m[i]);
		}
	}

	static inline constexpr DiagonalMatrix<T, Size> ZEROS() {
		DiagonalMatrix<T, Size> mat;
		for (std::size_t i = 0; i < Size; i++) {
			mat[i] = 0;
		}
		return mat;
	}

	static inline constexpr DiagonalMatrix<T, Size> IDENTITY() {
		DiagonalMatrix<T, Size> mat;
		for (std::size_t i = 0; i < Size; i++) {
			mat[i] = 1;
		}
		return mat;
	}

	operator SymmetricMatrix<T, Size>() const {
		SymmetricMatrix<T, Size> mat;
		for (std::size_t row = 0; row < Size; row++) {
			for (std::size_t col = 0; col < row; col++) {
				mat[row][col] = 0;
			}
			mat[row][row] = this->data[row];
		}
		return mat;
	}

	operator Matrix<T, Size, Size>() const {
		Matrix<T, Size, Size> mat;
		for (std::size_t row = 0; row < Size; row++) {
			for (std::size_t col = 0; col < row; col++) {
				mat[row][col] = 0;
				mat[col][row] = 0;
			}
			mat[row][row] = (*this)[row];
		}
		return mat;
	}
};

template<typename T, std::size_t Size>
using SquareMatrix = Matrix<T, Size, Size>;

template<typename T, std::size_t Size>
using UnitaryMatrix = SquareMatrix<T, Size>;


/*
	===== Predefined matrices
*/

template<typename T, std::size_t Size>
using RotationMatrix = Matrix<T, Size, Size>;

// Mat2
typedef Matrix<double, 2, 2>	Mat2;
typedef Matrix<float, 2, 2>		Mat2f;
typedef Matrix<long long, 2, 2>	Mat2l;
typedef Matrix<double, 2, 2>	RotMat2;

// Mat3
typedef Matrix<double, 3, 3>	Mat3;
typedef Matrix<float, 3, 3>		Mat3f;
typedef Matrix<long long, 3, 3>	Mat3l;

typedef SymmetricMatrix<double, 3>		SymmetricMat3;
typedef SymmetricMatrix<float, 3>		SymmetricMat3f;
typedef SymmetricMatrix<long long, 3>	SymmetricMat3l;

typedef DiagonalMatrix<double, 3>		DiagonalMat3;
typedef DiagonalMatrix<float, 3>			DiagonalMat3f;
typedef DiagonalMatrix<long long, 3>		DiagonalMat3l;

// Mat4
typedef Matrix<double, 4, 4>	Mat4;
typedef Matrix<float, 4, 4>		Mat4f;
typedef Matrix<long long, 4, 4>	Mat4l;





/*
	===== Operators =====
*/

template<typename T, std::size_t Size>
T det(const Matrix<T, Size, Size>& matrix) {
	T total = 0;

	Matrix<T, Size, Size - 1> allButFirstRow = matrix.withoutRow(0);

	for (std::size_t col = 0; col < Size; col++) {
		T detOfMinor = det(allButFirstRow.withoutCol(col));
		
		T value = detOfMinor * matrix[0][col];

		total += (col % 2 == 0)? value : -value;
	}
	return total;
}
template<typename T>
T det(const Matrix<T, 0, 0> & matrix) {
	return 1; // for shits n giggles
}
template<typename T>
T det(const Matrix<T, 1, 1>& matrix) {
	return matrix[0][0];
}
template<typename T>
T det(const Matrix<T, 2, 2> & matrix) {
	return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
}
template<typename T, std::size_t Size>
T det(const SymmetricMatrix<T, Size>& mat) {
	return det(static_cast<Matrix<T, Size, Size>>(mat));
}
template<typename T, std::size_t Size>
T det(const DiagonalMatrix<T, Size>& mat) {
	T total = mat[0];

	for (std::size_t i = 1; i < Size; i++) {
		total *= mat[i];
	}
	return total;
}
template<typename T>
T det(const DiagonalMatrix<T, 0>& mat) {
	return 1;
}

template<typename T, std::size_t Height, std::size_t Width1, std::size_t Width2>
Matrix<T, Width1 + Width2, Height> joinHorizontal(const Matrix<T, Width1, Height>& mat1, const Matrix<T, Width2, Height>& mat2) {
	Matrix<T, Width1 + Width2, Height> result;
	for (std::size_t row = 0; row < Height; row++) {
		for (std::size_t col = 0; col < Width1; col++) {
			result[row][col] = mat1[row][col];
		}
		for (std::size_t col = 0; col < Width2; col++) {
			result[row][col + Width1] = mat2[row][col];
		}
	}
	return result;
}

template<typename T, std::size_t Width, std::size_t Height1, std::size_t Height2>
Matrix<T, Width, Height1 + Height2> joinVertical(const Matrix<T, Width, Height1>& mat1, const Matrix<T, Width, Height2>& mat2) {
	Matrix<T, Width, Height1 + Height2> result;
	for (std::size_t row = 0; row < Height1; row++) {
		for (std::size_t col = 0; col < Width; col++) {
			result[row][col] = mat1[row][col];
		}
	}
	for (std::size_t row = 0; row < Height2; row++) {
		for (std::size_t col = 0; col < Width; col++) {
			result[row + Height1][col] = mat2[row][col];
		}
	}
	return result;
}

/*
	===== Everything standard matrix =====
*/

template<typename T, std::size_t Width, std::size_t Height>
Matrix<T, Width, Height> operator+(const Matrix<T, Width, Height>& m1, const Matrix<T, Width, Height>& m2) {
	Matrix<T, Width, Height> result;

	for (std::size_t row = 0; row < Height; row++) {
		for (std::size_t col = 0; col < Width; col++) {
			result[row][col] = m1[row][col] + m2[row][col];
		}
	}
	return result;
}

template<typename T, std::size_t Width, std::size_t Height>
Matrix<T, Width, Height> operator-(const Matrix<T, Width, Height>& m1, const Matrix<T, Width, Height>& m2) {
	Matrix<T, Width, Height> result;

	for (std::size_t row = 0; row < Height; row++) {
		for (std::size_t col = 0; col < Width; col++) {
			result[row][col] = m1[row][col] - m2[row][col];
		}
	}
	return result;
}

template<typename T, std::size_t Width, std::size_t Height>
Matrix<T, Width, Height> operator-(const Matrix<T, Width, Height>& m) {
	Matrix<T, Width, Height> result;

	for (std::size_t row = 0; row < Height; row++) {
		for (std::size_t col = 0; col < Width; col++) {
			result[row][col] = -m[row][col];
		}
	}
	return result;
}

template<typename T, std::size_t Width, std::size_t Height>
Vector<T, Height> operator*(const Matrix<T, Width, Height>& m, const Vector<T, Width>& v) {
	Vector<T, Height> result;

	for (std::size_t row = 0; row < Height; row++) {
		T sum = m[row][0] * v[0];
		for (std::size_t col = 1; col < Width; col++) {
			sum += m[row][col] * v[col];
		}
		result[row] = sum;
	}
	return result;
}

template<typename T>
Matrix<T, 3, 3> operator*(const Matrix<T, 3, 3>& m, const Matrix<T, 3, 3> & m2) {
	Matrix<T, 3, 3> result;
	result.data[0] = m.data[0] * m2.data[0] + m.data[3] * m2.data[1] + m.data[6] * m2.data[2];
	result.data[1] = m.data[1] * m2.data[0] + m.data[4] * m2.data[1] + m.data[7] * m2.data[2];
	result.data[2] = m.data[2] * m2.data[0] + m.data[5] * m2.data[1] + m.data[8] * m2.data[2];

	result.data[3] = m.data[0] * m2.data[3] + m.data[3] * m2.data[4] + m.data[6] * m2.data[5];
	result.data[4] = m.data[1] * m2.data[3] + m.data[4] * m2.data[4] + m.data[7] * m2.data[5];
	result.data[5] = m.data[2] * m2.data[3] + m.data[5] * m2.data[4] + m.data[8] * m2.data[5];
	
	result.data[6] = m.data[0] * m2.data[6] + m.data[3] * m2.data[7] + m.data[6] * m2.data[8];
	result.data[7] = m.data[1] * m2.data[6] + m.data[4] * m2.data[7] + m.data[7] * m2.data[8];
	result.data[8] = m.data[2] * m2.data[6] + m.data[5] * m2.data[7] + m.data[8] * m2.data[8];

	return result;
}

template<typename T>
Vector<T, 3> operator*(const Matrix<T, 3, 3>& m, const Vector<T, 3>& v) {
	T x = m.data[0] * v[0] + m.data[3] * v[1] + m.data[6] * v[2];
	T y = m.data[1] * v[0] + m.data[4] * v[1] + m.data[7] * v[2];
	T z = m.data[2] * v[0] + m.data[5] * v[1] + m.data[8] * v[2];

	return Vector<T, 3>(x, y, z);
}

template<typename T, std::size_t ResultWidth, std::size_t ResultHeight, std::size_t IntermediateSize>
Matrix<T, ResultWidth, ResultHeight> operator*(const Matrix<T, IntermediateSize, ResultHeight>& m1, const Matrix<T, ResultWidth, IntermediateSize>& m2) {
	Matrix<T, ResultWidth, ResultHeight> result;

	for (std::size_t col = 0; col < ResultWidth; col++) {
		for (std::size_t row = 0; row < ResultHeight; row++) {
			T sum = m1[row][0] * m2[0][col];
			for (std::size_t i = 1; i < IntermediateSize; i++) {
				sum += m1[row][i] * m2[i][col];
			}
			result[row][col] = sum;
		}
	}
	return result;
}

template<typename T, std::size_t Width, std::size_t Height>
Matrix<T, Width, Height> operator*(const Matrix<T, Width, Height>& m1, const T& factor) {
	Matrix<T, Width, Height> result;

	for (std::size_t row = 0; row < Height; row++) {
		for (std::size_t col = 0; col < Width; col++) {
			result[row][col] = m1[row][col] * factor;
		}
	}
	return result;
}

template<typename T, std::size_t Width, std::size_t Height>
Matrix<T, Width, Height> operator*(const T& factor, const Matrix<T, Width, Height>& m1) {
	Matrix<T, Width, Height> result;

	for (std::size_t row = 0; row < Height; row++) {
		for (std::size_t col = 0; col < Width; col++) {
			result[row][col] = factor * m1[row][col];
		}
	}
	return result;
}

template<typename T, std::size_t Width, std::size_t Height>
Matrix<T, Width, Height> operator/(const Matrix<T, Width, Height>& m1, const T& factor) {
	Matrix<T, Width, Height> result;

	for (std::size_t row = 0; row < Height; row++) {
		for (std::size_t col = 0; col < Width; col++) {
			result[row][col] = m1[row][col] / factor;
		}
	}
	return result;
}

template<typename T, std::size_t Width, std::size_t Height>
Matrix<T, Width, Height>& operator+=(Matrix<T, Width, Height>& m1, const Matrix<T, Width, Height>& m2) {
	for (std::size_t row = 0; row < Height; row++) {
		for (std::size_t col = 0; col < Width; col++) {
			m1[row][col] += m2[row][col];
		}
	}
	return m1;
}
template<typename T, std::size_t Width, std::size_t Height>
Matrix<T, Width, Height>& operator-=(Matrix<T, Width, Height>& m1, const Matrix<T, Width, Height>& m2) {
	for (std::size_t row = 0; row < Height; row++) {
		for (std::size_t col = 0; col < Width; col++) {
			m1[row][col] -= m2[row][col];
		}
	}
	return m1;
}
template<typename T, std::size_t Width, std::size_t Height>
Matrix<T, Width, Height>& operator*=(Matrix<T, Width, Height>& mat, const T& factor) {
	for (std::size_t row = 0; row < Height; row++) {
		for (std::size_t col = 0; col < Width; col++) {
			mat[row][col] *= factor;
		}
	}
	return mat;
}
template<typename T, std::size_t Width, std::size_t Height>
Matrix<T, Width, Height>& operator/=(Matrix<T, Width, Height>& mat, const T& factor) {
	for (std::size_t row = 0; row < Height; row++) {
		for (std::size_t col = 0; col < Width; col++) {
			mat[row][col] /= factor;
		}
	}
	return mat;
}

template<typename T, std::size_t Size>
Matrix<T, Size, Size> operator~(const Matrix<T, Size, Size>& matrix) {
	Matrix<T, Size, Size> result;

	T d = det(matrix);

	for (std::size_t row = 0; row < Size; row++) {
		for (std::size_t col = 0; col < Size; col++) {
			Matrix<T, Size - 1, Size - 1> tmp = matrix.withoutRowCol(row, col);
			T coFactor = det(tmp);

			T value = coFactor / d;

			// transpose the coFactors here, therefore indexing [col][row]
			result[col][row] = ((row + col) % 2 == 0) ? value : -value;
		}
	}
	return result;
}

template<typename T, std::size_t Size>
Matrix<T, Size, Size> inverse(const Matrix<T, Size, Size>& matrix) {
	return ~matrix;
}

/*
	===== Everything symmetric matrix ===== 
*/


template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> operator+(const SymmetricMatrix<T, Size>& m1, const SymmetricMatrix<T, Size>& m2) {
	SymmetricMatrix<T, Size> result;

	for (std::size_t row = 0; row < Size; row++) {
		for (std::size_t col = 0; col <= row; col++) {
			result[row][col] = m1[row][col] + m2[row][col];
		}
	}
	return result;
}

template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> operator-(const SymmetricMatrix<T, Size>& m1, const SymmetricMatrix<T, Size>& m2) {
	SymmetricMatrix<T, Size> result;

	for (std::size_t row = 0; row < Size; row++) {
		for (std::size_t col = 0; col <= row; col++) {
			result[row][col] = m1[row][col] - m2[row][col];
		}
	}
	return result;
}

template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> operator-(const SymmetricMatrix<T, Size>& m) {
	SymmetricMatrix<T, Size> result;

	for (std::size_t row = 0; row < Size; row++) {
		for (std::size_t col = 0; col <= row; col++) {
			result[row][col] = -m[row][col];
		}
	}
	return result;
}

template<typename T, std::size_t Size>
Vector<T, Size> operator*(const SymmetricMatrix<T, Size>& m, const Vector<T, Size>& v) {
	Vector<T, Size> result;

	for (std::size_t row = 0; row < Size; row++) {
		T sum = m[row][0] * v[0];
		for (std::size_t col = 1; col < Size; col++) {
			sum += m[row][col] * v[col];
		}
		result[row] = sum;
	}
	return result;
}

template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> operator*(const SymmetricMatrix<T, Size>& m1, const SymmetricMatrix<T, Size>& m2) {
	SymmetricMatrix<T, Size> result;

	for (std::size_t col = 0; col < Size; col++) {
		for (std::size_t row = 0; row <= col; row++) {
			T sum = m1[row][0] * m2[0][col];
			for (std::size_t i = 1; i < Size; i++) {
				sum += m1[row][i] * m2[i][col];
			}
			result[row][col] = sum;
		}
	}
	return result;
}

template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> operator*(const SymmetricMatrix<T, Size>& m1, const T& factor) {
	SymmetricMatrix<T, Size> result;

	for (std::size_t row = 0; row < Size; row++) {
		for (std::size_t col = 0; col <= row; col++) {
			result[row][col] = m1[row][col] * factor;
		}
	}
	return result;
}

template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> operator*(const T& factor, const SymmetricMatrix<T, Size>& m1) {
	SymmetricMatrix<T, Size> result;

	for (std::size_t row = 0; row < Size; row++) {
		for (std::size_t col = 0; col <= row; col++) {
			result[row][col] = factor * m1[row][col];
		}
	}
	return result;
}

template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> operator/(const SymmetricMatrix<T, Size>& m1, const T& factor) {
	SymmetricMatrix<T, Size> result;

	for (std::size_t row = 0; row < Size; row++) {
		for (std::size_t col = 0; col <= row; col++) {
			result[row][col] = m1[row][col] / factor;
		}
	}
	return result;
}

template<typename T, std::size_t Size>
SymmetricMatrix<T, Size>& operator+=(SymmetricMatrix<T, Size>& m1, const SymmetricMatrix<T, Size>& m2) {
	for (std::size_t row = 0; row < Size; row++) {
		for (std::size_t col = 0; col <= row; col++) {
			m1[row][col] += m2[row][col];
		}
	}
	return m1;
}
template<typename T, std::size_t Size>
SymmetricMatrix<T, Size>& operator-=(SymmetricMatrix<T, Size>& m1, const SymmetricMatrix<T, Size>& m2) {
	for (std::size_t row = 0; row < Size; row++) {
		for (std::size_t col = 0; col <= row; col++) {
			m1[row][col] -= m2[row][col];
		}
	}
	return m1;
}
template<typename T, std::size_t Size>
SymmetricMatrix<T, Size>& operator*=(SymmetricMatrix<T, Size>& mat, const T& factor) {
	for (std::size_t row = 0; row < Size; row++) {
		for (std::size_t col = 0; col <= row; col++) {
			mat[row][col] *= factor;
		}
	}
	return mat;
}
template<typename T, std::size_t Size>
SymmetricMatrix<T, Size>& operator/=(SymmetricMatrix<T, Size>& mat, const T& factor) {
	for (std::size_t row = 0; row < Size; row++) {
		for (std::size_t col = 0; col <= row; col++) {
			mat[row][col] /= factor;
		}
	}
	return mat;
}

template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> operator~(const SymmetricMatrix<T, Size>& matrix) {
	SymmetricMatrix<T, Size> result;

	T d = det(matrix);

	for (std::size_t row = 0; row < Size; row++) {
		for (std::size_t col = 0; col <= row; col++) {
			Matrix<T, Size - 1, Size - 1> tmp = static_cast<Matrix<T, Size, Size>>(matrix).withoutRowCol(row, col);
			T coFactor = det(tmp);

			T value = coFactor / d;

			// transpose the coFactors here, therefore indexing [col][row]
			result[col][row] = ((row + col) % 2 == 0) ? value : -value;
		}
	}
	return result;
}

template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> inverse(const SymmetricMatrix<T, Size>& matrix) {
	return ~matrix;
}

/*
	===== Everything Diagonal Matrix =====
*/

template<typename T, std::size_t Size>
DiagonalMatrix<T, Size> operator+(const DiagonalMatrix<T, Size>& m1, const DiagonalMatrix<T, Size>& m2) {
	DiagonalMatrix<T, Size> result;

	for (std::size_t i = 0; i < Size; i++) {
		result[i] = m1[i] + m2[i];
	}
	return result;
}

template<typename T, std::size_t Size>
DiagonalMatrix<T, Size> operator-(const DiagonalMatrix<T, Size>& m1, const DiagonalMatrix<T, Size>& m2) {
	DiagonalMatrix<T, Size> result;

	for (std::size_t i = 0; i < Size; i++) {
		result[i] = m1[i] - m2[i];
	}
	return result;
}

template<typename T, std::size_t Size>
DiagonalMatrix<T, Size> operator-(const DiagonalMatrix<T, Size>& m) {
	DiagonalMatrix<T, Size> result;

	for (std::size_t i = 0; i < Size; i++) {
		result[i] = -m[i];
	}
	return result;
}

template<typename T, std::size_t Size>
Vector<T, Size> operator*(const DiagonalMatrix<T, Size>& m, const Vector<T, Size>& v) {
	Vector<T, Size> result;

	for (std::size_t i = 0; i < Size; i++) {
		result[i] = m[i] * v[i];
	}
	return result;
}

template<typename T, std::size_t Size>
DiagonalMatrix<T, Size> operator*(const DiagonalMatrix<T, Size>& m1, const DiagonalMatrix<T, Size>& m2) {
	DiagonalMatrix<T, Size> result;

	for (std::size_t i = 0; i < Size; i++) {
		result[i] = m1[i] * m2[i];
	}
	return result;
}

template<typename T, std::size_t Size>
DiagonalMatrix<T, Size> operator*(const DiagonalMatrix<T, Size>& m1, const T& factor) {
	DiagonalMatrix<T, Size> result;

	for (std::size_t i = 0; i < Size; i++) {
			result[i] = m1[i] * factor;
	}
	return result;
}

template<typename T, std::size_t Size>
DiagonalMatrix<T, Size> operator*(const T& factor, const DiagonalMatrix<T, Size>& m1) {
	DiagonalMatrix<T, Size> result;

	for (std::size_t i = 0; i < Size; i++) {
		result[i] = factor * m1[i];
	}
	return result;
}

template<typename T, std::size_t Size>
DiagonalMatrix<T, Size> operator/(const DiagonalMatrix<T, Size>& m1, const T& factor) {
	DiagonalMatrix<T, Size> result;

	for (std::size_t i = 0; i < Size; i++) {
		result[i] = m1[i] / factor;
	}
	return result;
}

template<typename T, std::size_t Size>
DiagonalMatrix<T, Size>& operator+=(DiagonalMatrix<T, Size>& m1, const DiagonalMatrix<T, Size>& m2) {
	for (std::size_t i = 0; i < Size; i++) {
		m1[i] += m2[i];
	}
	return m1;
}
template<typename T, std::size_t Size>
DiagonalMatrix<T, Size>& operator-=(DiagonalMatrix<T, Size>& m1, const DiagonalMatrix<T, Size>& m2) {
	for (std::size_t i = 0; i < Size; i++) {
		m1[i] -= m2[i];
	}
	return m1;
}
template<typename T, std::size_t Size>
DiagonalMatrix<T, Size>& operator*=(DiagonalMatrix<T, Size>& mat, const T& factor) {
	for (std::size_t i = 0; i < Size; i++) {
		mat[i] *= factor;
	}
	return mat;
}
template<typename T, std::size_t Size>
DiagonalMatrix<T, Size>& operator/=(DiagonalMatrix<T, Size>& mat, const T& factor) {
	for (std::size_t i = 0; i < Size; i++) {
		mat[i] /= factor;
	}
	return mat;
}

template<typename T, std::size_t Size>
DiagonalMatrix<T, Size> operator~(const DiagonalMatrix<T, Size>& matrix) {
	DiagonalMatrix<T, Size> result;

	for (std::size_t i = 0; i < Size; i++) {
		result[i] = 1 / matrix[i];
	}
	return result;
}

template<typename T, std::size_t Size>
DiagonalMatrix<T, Size> inverse(const DiagonalMatrix<T, Size>& matrix) {
	return ~matrix;
}

/*
	===== Compatibility between the matrix types =====
*/

template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> operator*(const SymmetricMatrix<T, Size>& m1, const DiagonalMatrix<T, Size>& m2) {
	return m1 * static_cast<SymmetricMatrix<T, Size>>(m2);
}
template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> operator*(const DiagonalMatrix<T, Size>& m1, const SymmetricMatrix<T, Size>& m2) {
	return static_cast<SymmetricMatrix<T, Size>>(m1) * m2;
}

template<typename T, std::size_t Size, std::size_t Width>
Matrix<T, Width, Size> operator*(const SymmetricMatrix<T, Size>& m1, const Matrix<T, Width, Size>& m2) {
	return static_cast<Matrix<T, Size, Size>>(m1)* m2;
}
template<typename T, std::size_t Size, std::size_t Height>
Matrix<T, Size, Height> operator*(const Matrix<T, Size, Height>& m1, const SymmetricMatrix<T, Size>& m2) {
	return m1 * static_cast<Matrix<T, Size, Size>>(m2);
}

template<typename T, std::size_t Size, std::size_t Width>
Matrix<T, Width, Size> operator*(const DiagonalMatrix<T, Size>& m1, const Matrix<T, Width, Size>& m2) {
	return static_cast<Matrix<T, Size, Size>>(m1)* m2;
}
template<typename T, std::size_t Size, std::size_t Height>
Matrix<T, Size, Height> operator*(const Matrix<T, Size, Height>& m1, const DiagonalMatrix<T, Size>& m2) {
	return m1 * static_cast<Matrix<T, Size, Size>>(m2);
}


/*
	===== Other functions =====
*/

template<typename T, std::size_t Width, std::size_t Height>
Matrix<T, Width, Height> outer(const Vector<T, Height>& v1, const Vector<T, Width>& v2) {
	Matrix<T, Width, Height> result;

	for (std::size_t row = 0; row < Height; row++) {
		for (std::size_t col = 0; col < Width; col++) {
			result[row][col] = v1[row] * v2[col];
		}
	}

	return result;
}

template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> selfOuter(const Vector<T, Size>& v) {
	SymmetricMatrix<T, Size> result;

	for (std::size_t row = 0; row < Size; row++) {
		for (std::size_t col = 0; col <= row; col++) {
			result[row][col] = v[row] * v[col];
		}
	}
	return result;
}


template<typename T, std::size_t Size>
Vector<T, Size> toVector(const Matrix<T, 1, Size>& mat) {
	Vector<T, Size> result;
	for(std::size_t i = 0; i < Size; i++) {
		result[i] = mat[i][0];
	}
	return result;
}
template<typename T, std::size_t Size>
Vector<T, Size> toVector(const Matrix<T, Size, 1>& mat) {
	Vector<T, Size> result;
	for(std::size_t i = 0; i < Size; i++) {
		result[i] = mat[0][i];
	}
	return result;
}

template<typename T, std::size_t Size>
Matrix<T, 1, Size> toRowMatrix(const Vector<T, Size>& vec) {
	Matrix<T, 1, Size> result;
	for(std::size_t i = 0; i < Size; i++) {
		result[0][i] = vec[i];
	}
	return result;
}
template<typename T, std::size_t Size>
Matrix<T, Size, 1> toColMatrix(const Vector<T, Size>& vec) {
	Matrix<T, Size, 1> result;
	for(std::size_t i = 0; i < Size; i++) {
		result[i][0] = vec[i];
	}
	return result;
}

/*
	Computes mat + mat.transpose()
*/
template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> addTransposed(const SquareMatrix<T, Size>& mat) {
	SymmetricMatrix<T, Size> result;
	for(std::size_t resultRow = 0; resultRow < Size; resultRow++) {
		for(std::size_t resultCol = 0; resultCol <= resultRow; resultCol++) {
			result[resultRow][resultCol] = mat[resultRow][resultCol] + mat[resultCol][resultRow];
		}
	}
	return result;
}

/*
	computes 
	m * sym * m.transpose()
*/
template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> mulSymmetricLeftRightTranspose(const SymmetricMatrix<T, Size>& sym, const SquareMatrix<T, Size>& m) {
	SquareMatrix<T, Size> symResult = m * sym * m.transpose();
	SymmetricMatrix<T, Size> result;
	for(std::size_t resultRow = 0; resultRow < Size; resultRow++) {
		for(std::size_t resultCol = 0; resultCol <= resultRow; resultCol++) {
			result[resultRow][resultCol] = symResult[resultRow][resultCol];
		}
	}
	return result;
}
/*
	computes
	m.transpose() * sym * m
*/
template<typename T, std::size_t Size>
SymmetricMatrix<T, Size> mulSymmetricLeftTransposeRight(const SymmetricMatrix<T, Size>& sym, const SquareMatrix<T, Size>& m) {
	SquareMatrix<T, Size> symResult = m.transpose() * sym * m;
	SymmetricMatrix<T, Size> result;
	for(std::size_t resultRow = 0; resultRow < Size; resultRow++) {
		for(std::size_t resultCol = 0; resultCol <= resultRow; resultCol++) {
			result[resultRow][resultCol] = symResult[resultRow][resultCol];
		}
	}
	return result;
}
