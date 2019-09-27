#pragma once

#include "vec.h"
#include <initializer_list>
#include <assert.h>

template<typename T, size_t Width, size_t Height>
struct MatrixIndex {
	T* data;
	size_t row;
	inline constexpr T& operator[](size_t col) const {
		assert(row >= 0 && row < Height);
		assert(col >= 0 && col < Width);

		return data[col * Height + row]; 
	}
};
template<typename T, size_t Width, size_t Height>
class Matrix {
	T data[Width * Height];

public:
	// Returns a row of this matrix
	inline constexpr MatrixIndex<T, Width, Height> operator[](size_t row) { return MatrixIndex<T, Width, Height>{ data, row }; }
	// Returns a row of this matrix
	inline constexpr MatrixIndex<const T, Width, Height> operator[](size_t col) const { return MatrixIndex<const T, Width, Height>{ data, col }; }

	Matrix<T, Width, Height>() = default;

	/*
		Initialize matrices like so

		Matrix<T, 4, 3>{
			1, 2, 3, 4,
			5, 6, 7, 8,
			9, 10, 11, 12
		};
	*/
	inline constexpr Matrix<T, Width, Height>(const std::initializer_list<T>& list) {
		assert(list.size() == Width * Height);
		auto listIter = list.begin();
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
				(*this)[row][col] = *listIter;
				++listIter;
			}
		}
	}

	template<typename OtherT1, typename OtherT2, typename OtherT3, typename OtherT4>
	Matrix<T, Width, Height>(const Matrix<OtherT1, Width - 1, Height - 1>& topLeftMat, const Vector<OtherT2, Height - 1>& rightCol, const Vector<OtherT3, Width - 1>& bottomRow, const OtherT4& bottomLeftVal) {
		for (size_t row = 0; row < Height - 1; row++) {
			for (size_t col = 0; col < Width - 1; col++) {
				(*this)[row][col] = topLeftMat[row][col];
			}
			(*this)[row][Width - 1] = rightCol[row];
		}
		for (size_t col = 0; col < Width - 1; col++) {
			(*this)[Height - 1][col] = bottomRow[col];
		}
		(*this)[Height - 1][Width - 1] = bottomLeftVal;
	}

	template<typename OtherT1, typename OtherT2>
	Matrix<T, Width, Height>(const Matrix<OtherT1, Width - 1, Height - 1> & topLeftMat, const OtherT2& bottomLeftVal) {
		for (size_t row = 0; row < Height - 1; row++) {
			for (size_t col = 0; col < Width - 1; col++) {
				(*this)[row][col] = topLeftMat[row][col];
			}
			(*this)[row][Width - 1] = 0.0;
		}
		for (size_t col = 0; col < Width - 1; col++) {
			(*this)[Height - 1][col] = 0.0;
		}
		(*this)[Height - 1][Width - 1] = bottomLeftVal;
	}

	void setDataRowMajor(const T* data) {
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
				(*this)[row][col] = data[row * Width + col];
			}
		}
	}
	void setDataColMajor(const T* data) {
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
				(*this)[row][col] = data[row + col * Height];
			}
		}
	}

	template<typename T2>
	Matrix<T, Width, Height>(const Matrix<T2, Width, Height>& mat) {
		for (size_t row = 0; row < Height; row++) {
			for(size_t col = 0; col < Width; col++)
			(*this)[row][col] = static_cast<T>(mat[row][col]);
		}
	}

	Matrix<T, Height, Width> transpose() const {
		Matrix<T, Height, Width> result;
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
				result[col][row] = (*this)[row][col];
			}
		}
		return result;
	}

	template<size_t SubWidth, size_t SubHeight>
	Matrix<T, SubWidth, SubHeight> getSubMatrix(size_t topLeftRow, size_t topLeftCol) const {
		assert(topLeftRow >= 0 && topLeftRow + SubHeight <= Height);
		assert(topLeftCol >= 0 && topLeftCol + SubWidth <= Width);

		Matrix<T, SubWidth, SubHeight> result;

		for (size_t row = 0; row < SubHeight; row++) {
			for (size_t col = 0; col < SubWidth; col++) {
				result[row][col] = (*this)[row+topLeftRow][col+topLeftCol];
			}
		}
		return result;
	}

	template<size_t SubWidth, size_t SubHeight>
	void setSubMatrix(const Matrix<T, SubWidth, SubHeight>& mat, size_t topLeftRow, size_t topLeftCol) {
		assert(topLeftRow >= 0 && topLeftRow + SubHeight <= Height);
		assert(topLeftCol >= 0 && topLeftCol + SubWidth <= Width);

		for (size_t row = 0; row < SubHeight; row++) {
			for (size_t col = 0; col < SubWidth; col++) {
				(*this)[row + topLeftRow][col + topLeftCol] = mat[row][col];
			}
		}
	}

	Matrix<T, Width - 1, Height> withoutCol(size_t colToDelete) const {
		assert(colToDelete >= 0 && colToDelete < Width);

		Matrix<T, Width - 1, Height> newMat;
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < colToDelete; col++) {
				newMat[row][col] = (*this)[row][col];
			}
			for (size_t col = colToDelete+1; col < Width; col++) {
				newMat[row][col - 1] = (*this)[row][col];
			}
		}
		return newMat;
	}
	Matrix<T, Width, Height - 1> withoutRow(size_t rowToDelete) const {
		assert(rowToDelete >= 0 && rowToDelete < Height);

		Matrix<T, Width, Height - 1> newMat;
		for (size_t col = 0; col < Width; col++) {
			for (size_t row = 0; row < rowToDelete; row++) {
				newMat[row][col] = (*this)[row][col];
			}
			for (size_t row = rowToDelete; row < Height - 1; row++) {
				newMat[row][col] = (*this)[row + 1][col];
			}
		}

		return newMat;
	}
	Matrix<T, Width - 1, Height - 1> withoutRowCol(size_t rowToDelete, size_t colToDelete) const {
		assert(colToDelete >= 0 && colToDelete < Width);
		assert(rowToDelete >= 0 && rowToDelete < Height);

		Matrix<T, Width - 1, Height - 1> newMat;
		for (size_t row = 0; row < rowToDelete; row++) {
			for (size_t col = 0; col < colToDelete; col++) {
				newMat[row][col] = (*this)[row][col];
			}
			for (size_t col = colToDelete + 1; col < Width; col++) {
				newMat[row][col - 1] = (*this)[row][col];
			}
		}
		for (size_t row = rowToDelete + 1; row < Height; row++) {
			for (size_t col = 0; col < colToDelete; col++) {
				newMat[row - 1][col] = (*this)[row][col];
			}
			for (size_t col = colToDelete + 1; col < Width; col++) {
				newMat[row - 1][col - 1] = (*this)[row][col];
			}
		}
		return newMat;
	}
	
	void setRow(size_t row, const Vector<T, Width>& data) {
		assert(row >= 0 && row < Height);

		for (size_t i = 0; i < Width; i++) {
			(*this)[row][i] = data[i];
		}
	}
	void setCol(size_t col, const Vector<T, Height>& data) {
		assert(col >= 0 && col < Width);

		for (size_t i = 0; i < Height; i++) {
			(*this)[i][col] = data[i];
		}
	}

	static inline constexpr Matrix<T, Width, Height> ZEROS() {
		Matrix<T, Width, Height> mat;
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
				mat[row][col] = 0;
			}
		}
		return mat;
	}

	static inline constexpr Matrix<T, Width, Height> IDENTITY() {
		Matrix<T, Width, Height> mat;
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
				mat[row][col] = (row == col) ? 1 : 0;
			}
		}
		return mat;
	}


	static Matrix<T, Width, Height> fromColMajorData(const T* data) {
		Matrix<T, Width, Height> mat;
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
				mat[row][col] = data[row + col * Height];
			}
		}
		return mat;
	}

	static Matrix<T, Width, Height> fromRowMajorData(const T* data) {
		Matrix<T, Width, Height> mat;
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
				mat[row][col] = data[row * Width + col];
			}
		}
		return mat;
	}

	void toColMajorData(T* buf) const {
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
				buf[row + col * Height] = (*this)[row][col];
			}
		}
	}

	void toRowMajorData(T* buf) const {
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
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


template<typename T, size_t Size>
struct SymmetricMatrixIndex {
	T* data;
	size_t row;

	inline constexpr T& operator[](size_t col) const {
		size_t a = (row > col) ? row : col; // max
		size_t b = (row > col) ? col : row; // min

		assert(b >= 0);
		assert(a < Size);

		return data[a * (a + 1) / 2 + b];
	}
};
template<typename T, size_t Size>
class SymmetricMatrix {
	T data[Size * (Size + 1) / 2];
public:
	// Returns a row of this matrix
	inline constexpr SymmetricMatrixIndex<T, Size> operator[](size_t row) { return SymmetricMatrixIndex<T, Size>{ data, row }; }
	// Returns a row of this matrix
	inline constexpr SymmetricMatrixIndex<const T, Size> operator[](size_t row) const { return SymmetricMatrixIndex<const T, Size>{ data, row }; }

	SymmetricMatrix<T, Size>() = default;
	/*
		Initialize symmetric matrices like so

		SymmetricMatrix<T, 4>{
			1,
			2, 3,
			4, 5, 6,
			7, 8, 9, 10
		};
	*/
	inline constexpr SymmetricMatrix<T, Size>(const std::initializer_list<T>& list) {
		assert(list.size() == Size * (Size + 1) / 2);

		auto listIter = list.begin();
		for (size_t row = 0; row < Size; row++) {
			for (size_t col = 0; col <= row; col++) {
				(*this)[row][col] = *listIter;
				++listIter;
			}
		}
	}

	static inline constexpr SymmetricMatrix<T, Size> ZEROS() {
		SymmetricMatrix<T, Size> mat;
		for (size_t row = 0; row < Size; row++) {
			for (size_t col = 0; col <= row; col++) {
				mat[row][col] = 0;
			}
		}
		return mat;
	}

	static inline constexpr SymmetricMatrix<T, Size> IDENTITY() {
		SymmetricMatrix<T, Size> mat;
		for (size_t row = 0; row < Size; row++) {
			for (size_t col = 0; col <= row; col++) {
				mat[row][col] = (row == col) ? 1 : 0;
			}
		}
		return mat;
	}

	operator Matrix<T, Size, Size>() const {
		Matrix<T, Size, Size> mat;
		for (size_t row = 0; row < Size; row++) {
			for (size_t col = 0; col <= row; col++) {
				const T& val = (*this)[row][col];
				mat[row][col] = val;
				mat[col][row] = val;
			}
		}
		return mat;
	}
};

template<typename T, size_t Size>
class DiagonalMatrix {
	T data[Size];
public:
	inline constexpr T& operator[](size_t index) { assert(index >= 0 && index < Size); return data[index]; }
	inline constexpr const T& operator[](size_t index) const { assert(index >= 0 && index < Size); return data[index]; }

	DiagonalMatrix<T, Size>() = default;
	/*
		Initialize diagonal matrices like so

		DiagonalMatrix<T, 4>{
			1,
			   2,
			      3,
			         4
		};
	*/
	inline constexpr DiagonalMatrix<T, Size>(const std::initializer_list<T>& list) {
		assert(list.size() == Size);

		auto listIter = list.begin();
		for (size_t i = 0; i < Size; i++) {
			(*this)[i] = *listIter;
			++listIter;
		}
	}

	static inline constexpr DiagonalMatrix<T, Size> ZEROS() {
		DiagonalMatrix<T, Size> mat;
		for (size_t i = 0; i < Size; i++) {
			mat[i] = 0;
		}
		return mat;
	}

	static inline constexpr DiagonalMatrix<T, Size> IDENTITY() {
		DiagonalMatrix<T, Size> mat;
		for (size_t i = 0; i < Size; i++) {
			mat[i] = 1;
		}
		return mat;
	}

	operator SymmetricMatrix<T, Size>() const {
		SymmetricMatrix<T, Size> mat;
		for (size_t row = 0; row < Size; row++) {
			for (size_t col = 0; col < row; col++) {
				mat[row][col] = 0;
			}
			mat[row][row] = this->data[row];
		}
		return mat;
	}

	operator Matrix<T, Size, Size>() const {
		Matrix<T, Size, Size> mat;
		for (size_t row = 0; row < Size; row++) {
			for (size_t col = 0; col < row; col++) {
				mat[row][col] = 0;
				mat[col][row] = 0;
			}
			mat[row][row] = (*this)[row];
		}
		return mat;
	}
};

template<typename T, size_t Size>
using SquareMatrix = Matrix<T, Size, Size>;

template<typename T, size_t Size>
using UnitaryMatrix = SquareMatrix<T, Size>;


/*
	===== Predefined matrices
*/

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

typedef Matrix<double, 3, 3>	RotMat3;
typedef Matrix<float, 3, 3>		RotMat3f;
typedef Matrix<long long, 3, 3>	RotMat3l;

// Mat4
typedef Matrix<double, 4, 4>	Mat4;
typedef Matrix<float, 4, 4>		Mat4f;
typedef Matrix<long long, 4, 4>	Mat4l;





/*
	===== Operators =====
*/

template<typename T, size_t Size>
T det(const Matrix<T, Size, Size>& matrix) {
	T total = 0;

	Matrix<T, Size, Size - 1> allButFirstRow = matrix.getSubMatrix<Size, Size-1>(1,0);

	for (size_t col = 0; col < Size; col++) {
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
template<typename T, size_t Size>
T det(const SymmetricMatrix<T, Size>& mat) {
	return det(static_cast<Matrix<T, Size, Size>>(mat));
}
template<typename T, size_t Size>
T det(const DiagonalMatrix<T, Size>& mat) {
	T total = mat[0];

	for (size_t i = 1; i < Size; i++) {
		total *= mat[i];
	}
	return total;
}
template<typename T>
T det(const DiagonalMatrix<T, 0>& mat) {
	return 1;
}

template<typename T, size_t Height, size_t Width1, size_t Width2>
Matrix<T, Width1 + Width2, Height> joinHorizontal(const Matrix<T, Width1, Height>& mat1, const Matrix<T, Width2, Height>& mat2) {
	Matrix<T, Width1 + Width2, Height> result;
	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width1; col++) {
			result[row][col] = mat1[row][col];
		}
		for (size_t col = 0; col < Width2; col++) {
			result[row][col + Width1] = mat2[row][col];
		}
	}
	return result;
}

template<typename T, size_t Width, size_t Height1, size_t Height2>
Matrix<T, Width, Height1 + Height2> joinVertical(const Matrix<T, Width, Height1>& mat1, const Matrix<T, Width, Height2>& mat2) {
	Matrix<T, Width, Height1 + Height2> result;
	for (size_t row = 0; row < Height1; row++) {
		for (size_t col = 0; col < Width; col++) {
			result[row][col] = mat1[row][col];
		}
	}
	for (size_t row = 0; row < Height2; row++) {
		for (size_t col = 0; col < Width; col++) {
			result[row + Height1][col] = mat2[row][col];
		}
	}
	return result;
}

/*
	===== Everything standard matrix =====
*/

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height> operator+(const Matrix<T, Width, Height>& m1, const Matrix<T, Width, Height>& m2) {
	Matrix<T, Width, Height> result;

	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width; col++) {
			result[row][col] = m1[row][col] + m2[row][col];
		}
	}
	return result;
}

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height> operator-(const Matrix<T, Width, Height>& m1, const Matrix<T, Width, Height>& m2) {
	Matrix<T, Width, Height> result;

	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width; col++) {
			result[row][col] = m1[row][col] - m2[row][col];
		}
	}
	return result;
}

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height> operator-(const Matrix<T, Width, Height>& m) {
	Matrix<T, Width, Height> result;

	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width; col++) {
			result[row][col] = -m[row][col];
		}
	}
	return result;
}

template<typename T, size_t Width, size_t Height>
Vector<T, Height> operator*(const Matrix<T, Width, Height>& m, const Vector<T, Width>& v) {
	Vector<T, Height> result;

	for (size_t row = 0; row < Height; row++) {
		T sum = m[row][0] * v[0];
		for (size_t col = 1; col < Width; col++) {
			sum += m[row][col] * v[col];
		}
		result[row] = sum;
	}
	return result;
}

template<typename T, size_t ResultWidth, size_t ResultHeight, size_t IntermediateSize>
Matrix<T, ResultWidth, ResultHeight> operator*(const Matrix<T, IntermediateSize, ResultHeight>& m1, const Matrix<T, ResultWidth, IntermediateSize>& m2) {
	Matrix<T, ResultWidth, ResultHeight> result;

	for (size_t col = 0; col < ResultWidth; col++) {
		for (size_t row = 0; row < ResultHeight; row++) {
			T sum = m1[row][0] * m2[0][col];
			for (size_t i = 1; i < IntermediateSize; i++) {
				sum += m1[row][i] * m2[i][col];
			}
			result[row][col] = sum;
		}
	}
	return result;
}

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height> operator*(const Matrix<T, Width, Height>& m1, const T& factor) {
	Matrix<T, Width, Height> result;

	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width; col++) {
			result[row][col] = m1[row][col] * factor;
		}
	}
	return result;
}

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height> operator*(const T& factor, const Matrix<T, Width, Height>& m1) {
	Matrix<T, Width, Height> result;

	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width; col++) {
			result[row][col] = factor * m1[row][col];
		}
	}
	return result;
}

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height> operator/(const Matrix<T, Width, Height>& m1, const T& factor) {
	Matrix<T, Width, Height> result;

	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width; col++) {
			result[row][col] = m1[row][col] / factor;
		}
	}
	return result;
}

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height>& operator+=(Matrix<T, Width, Height>& m1, const Matrix<T, Width, Height>& m2) {
	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width; col++) {
			m1[row][col] += m2[row][col];
		}
	}
	return m1;
}
template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height>& operator-=(Matrix<T, Width, Height>& m1, const Matrix<T, Width, Height>& m2) {
	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width; col++) {
			m1[row][col] -= m2[row][col];
		}
	}
	return m1;
}
template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height>& operator*=(Matrix<T, Width, Height>& mat, const T& factor) {
	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width; col++) {
			mat[row][col] *= factor;
		}
	}
	return mat;
}
template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height>& operator/=(Matrix<T, Width, Height>& mat, const T& factor) {
	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width; col++) {
			mat[row][col] /= factor;
		}
	}
	return mat;
}

template<typename T, size_t Size>
Matrix<T, Size, Size> operator~(const Matrix<T, Size, Size>& matrix) {
	Matrix<T, Size, Size> result;

	T d = det(matrix);

	for (size_t row = 0; row < Size; row++) {
		for (size_t col = 0; col < Size; col++) {
			Matrix<T, Size - 1, Size - 1> tmp = matrix.withoutRowCol(row, col);
			T coFactor = det(tmp);

			T value = coFactor / d;

			// transpose the coFactors here, therefore indexing [col][row]
			result[col][row] = ((row + col) % 2 == 0) ? value : -value;
		}
	}
	return result;
}


/*
	===== Everything symmetric matrix ===== 
*/


template<typename T, size_t Size>
SymmetricMatrix<T, Size> operator+(const SymmetricMatrix<T, Size>& m1, const SymmetricMatrix<T, Size>& m2) {
	SymmetricMatrix<T, Size> result;

	for (size_t row = 0; row < Size; row++) {
		for (size_t col = 0; col <= row; col++) {
			result[row][col] = m1[row][col] + m2[row][col];
		}
	}
	return result;
}

template<typename T, size_t Size>
SymmetricMatrix<T, Size> operator-(const SymmetricMatrix<T, Size>& m1, const SymmetricMatrix<T, Size>& m2) {
	SymmetricMatrix<T, Size> result;

	for (size_t row = 0; row < Size; row++) {
		for (size_t col = 0; col <= row; col++) {
			result[row][col] = m1[row][col] - m2[row][col];
		}
	}
	return result;
}

template<typename T, size_t Size>
SymmetricMatrix<T, Size> operator-(const SymmetricMatrix<T, Size>& m) {
	SymmetricMatrix<T, Size> result;

	for (size_t row = 0; row < Size; row++) {
		for (size_t col = 0; col <= row; col++) {
			result[row][col] = -m[row][col];
		}
	}
	return result;
}

template<typename T, size_t Size>
Vector<T, Size> operator*(const SymmetricMatrix<T, Size>& m, const Vector<T, Size>& v) {
	Vector<T, Size> result;

	for (size_t row = 0; row < Size; row++) {
		T sum = m[row][0] * v[0];
		for (size_t col = 1; col < Size; col++) {
			sum += m[row][col] * v[col];
		}
		result[row] = sum;
	}
	return result;
}

template<typename T, size_t Size>
SymmetricMatrix<T, Size> operator*(const SymmetricMatrix<T, Size>& m1, const SymmetricMatrix<T, Size>& m2) {
	SymmetricMatrix<T, Size> result;

	for (size_t col = 0; col < Size; col++) {
		for (size_t row = 0; row <= col; row++) {
			T sum = m1[row][0] * m2[0][col];
			for (size_t i = 1; i < Size; i++) {
				sum += m1[row][i] * m2[i][col];
			}
			result[row][col] = sum;
		}
	}
	return result;
}

template<typename T, size_t Size>
SymmetricMatrix<T, Size> operator*(const SymmetricMatrix<T, Size>& m1, const T& factor) {
	SymmetricMatrix<T, Size> result;

	for (size_t row = 0; row < Size; row++) {
		for (size_t col = 0; col <= row; col++) {
			result[row][col] = m1[row][col] * factor;
		}
	}
	return result;
}

template<typename T, size_t Size>
SymmetricMatrix<T, Size> operator*(const T& factor, const SymmetricMatrix<T, Size>& m1) {
	SymmetricMatrix<T, Size> result;

	for (size_t row = 0; row < Size; row++) {
		for (size_t col = 0; col <= row; col++) {
			result[row][col] = factor * m1[row][col];
		}
	}
	return result;
}

template<typename T, size_t Size>
SymmetricMatrix<T, Size> operator/(const SymmetricMatrix<T, Size>& m1, const T& factor) {
	SymmetricMatrix<T, Size> result;

	for (size_t row = 0; row < Size; row++) {
		for (size_t col = 0; col <= row; col++) {
			result[row][col] = m1[row][col] / factor;
		}
	}
	return result;
}

template<typename T, size_t Size>
SymmetricMatrix<T, Size>& operator+=(SymmetricMatrix<T, Size>& m1, const SymmetricMatrix<T, Size>& m2) {
	for (size_t row = 0; row < Size; row++) {
		for (size_t col = 0; col <= row; col++) {
			m1[row][col] += m2[row][col];
		}
	}
	return m1;
}
template<typename T, size_t Size>
SymmetricMatrix<T, Size>& operator-=(SymmetricMatrix<T, Size>& m1, const SymmetricMatrix<T, Size>& m2) {
	for (size_t row = 0; row < Size; row++) {
		for (size_t col = 0; col <= row; col++) {
			m1[row][col] -= m2[row][col];
		}
	}
	return m1;
}
template<typename T, size_t Size>
SymmetricMatrix<T, Size>& operator*=(SymmetricMatrix<T, Size>& mat, const T& factor) {
	for (size_t row = 0; row < Size; row++) {
		for (size_t col = 0; col <= row; col++) {
			mat[row][col] *= factor;
		}
	}
	return mat;
}
template<typename T, size_t Size>
SymmetricMatrix<T, Size>& operator/=(SymmetricMatrix<T, Size>& mat, const T& factor) {
	for (size_t row = 0; row < Size; row++) {
		for (size_t col = 0; col <= row; col++) {
			mat[row][col] /= factor;
		}
	}
	return mat;
}

template<typename T, size_t Size>
SymmetricMatrix<T, Size> operator~(const SymmetricMatrix<T, Size>& matrix) {
	SymmetricMatrix<T, Size> result;

	T d = det(matrix);

	for (size_t row = 0; row < Size; row++) {
		for (size_t col = 0; col <= row; col++) {
			Matrix<T, Size - 1, Size - 1> tmp = static_cast<Matrix<T, Size, Size>>(matrix).withoutRowCol(row, col);
			T coFactor = det(tmp);

			T value = coFactor / d;

			// transpose the coFactors here, therefore indexing [col][row]
			result[col][row] = ((row + col) % 2 == 0) ? value : -value;
		}
	}
	return result;
}

/*
	===== Everything Diagonal Matrix =====
*/

template<typename T, size_t Size>
DiagonalMatrix<T, Size> operator+(const DiagonalMatrix<T, Size>& m1, const DiagonalMatrix<T, Size>& m2) {
	DiagonalMatrix<T, Size> result;

	for (size_t i = 0; i < Size; i++) {
		result[i] = m1[i] + m2[i];
	}
	return result;
}

template<typename T, size_t Size>
DiagonalMatrix<T, Size> operator-(const DiagonalMatrix<T, Size>& m1, const DiagonalMatrix<T, Size>& m2) {
	DiagonalMatrix<T, Size> result;

	for (size_t i = 0; i < Size; i++) {
		result[i] = m1[i] - m2[i];
	}
	return result;
}

template<typename T, size_t Size>
DiagonalMatrix<T, Size> operator-(const DiagonalMatrix<T, Size>& m) {
	DiagonalMatrix<T, Size> result;

	for (size_t i = 0; i < Size; i++) {
		result[i] = -m[i];
	}
	return result;
}

template<typename T, size_t Size>
Vector<T, Size> operator*(const DiagonalMatrix<T, Size>& m, const Vector<T, Size>& v) {
	Vector<T, Size> result;

	for (size_t i = 0; i < Size; i++) {
		result[i] = m[i] * v[i];
	}
	return result;
}

template<typename T, size_t Size>
DiagonalMatrix<T, Size> operator*(const DiagonalMatrix<T, Size>& m1, const DiagonalMatrix<T, Size>& m2) {
	DiagonalMatrix<T, Size> result;

	for (size_t i = 0; i < Size; i++) {
		result[i] = m1[i] * m2[i];
	}
	return result;
}

template<typename T, size_t Size>
DiagonalMatrix<T, Size> operator*(const DiagonalMatrix<T, Size>& m1, const T& factor) {
	DiagonalMatrix<T, Size> result;

	for (size_t i = 0; i < Size; i++) {
			result[i] = m1[i] * factor;
	}
	return result;
}

template<typename T, size_t Size>
DiagonalMatrix<T, Size> operator*(const T& factor, const DiagonalMatrix<T, Size>& m1) {
	DiagonalMatrix<T, Size> result;

	for (size_t i = 0; i < Size; i++) {
		result[i] = factor * m1[i];
	}
	return result;
}

template<typename T, size_t Size>
DiagonalMatrix<T, Size> operator/(const DiagonalMatrix<T, Size>& m1, const T& factor) {
	DiagonalMatrix<T, Size> result;

	for (size_t i = 0; i < Size; i++) {
		result[i] = m1[i] / factor;
	}
	return result;
}

template<typename T, size_t Size>
DiagonalMatrix<T, Size>& operator+=(DiagonalMatrix<T, Size>& m1, const DiagonalMatrix<T, Size>& m2) {
	for (size_t i = 0; i < Size; i++) {
		m1[i] += m2[i];
	}
	return m1;
}
template<typename T, size_t Size>
DiagonalMatrix<T, Size>& operator-=(DiagonalMatrix<T, Size>& m1, const DiagonalMatrix<T, Size>& m2) {
	for (size_t i = 0; i < Size; i++) {
		m1[i] -= m2[i];
	}
	return m1;
}
template<typename T, size_t Size>
DiagonalMatrix<T, Size>& operator*=(DiagonalMatrix<T, Size>& mat, const T& factor) {
	for (size_t i = 0; i < Size; i++) {
		mat[i] *= factor;
	}
	return mat;
}
template<typename T, size_t Size>
DiagonalMatrix<T, Size>& operator/=(DiagonalMatrix<T, Size>& mat, const T& factor) {
	for (size_t i = 0; i < Size; i++) {
		mat[i] /= factor;
	}
	return mat;
}

template<typename T, size_t Size>
DiagonalMatrix<T, Size> operator~(const DiagonalMatrix<T, Size>& matrix) {
	DiagonalMatrix<T, Size> result;

	for (size_t i = 0; i < Size; i++) {
		result[i] = 1 / matrix[i];
	}
	return result;
}

/*
	===== Compatibility between the matrix types =====
*/

template<typename T, size_t Size>
SymmetricMatrix<T, Size> operator*(const SymmetricMatrix<T, Size>& m1, const DiagonalMatrix<T, Size>& m2) {
	return m1 * static_cast<SymmetricMatrix<T, Size>>(m2);
}
template<typename T, size_t Size>
SymmetricMatrix<T, Size> operator*(const DiagonalMatrix<T, Size>& m1, const SymmetricMatrix<T, Size>& m2) {
	return static_cast<SymmetricMatrix<T, Size>>(m1) * m2;
}

template<typename T, size_t Size, size_t Width>
Matrix<T, Width, Size> operator*(const SymmetricMatrix<T, Size>& m1, const Matrix<T, Width, Size>& m2) {
	return static_cast<Matrix<T, Size, Size>>(m1)* m2;
}
template<typename T, size_t Size, size_t Height>
Matrix<T, Size, Height> operator*(const Matrix<T, Size, Height>& m1, const SymmetricMatrix<T, Size>& m2) {
	return m1 * static_cast<Matrix<T, Size, Size>>(m2);
}

template<typename T, size_t Size, size_t Width>
Matrix<T, Width, Size> operator*(const DiagonalMatrix<T, Size>& m1, const Matrix<T, Width, Size>& m2) {
	return static_cast<Matrix<T, Size, Size>>(m1)* m2;
}
template<typename T, size_t Size, size_t Height>
Matrix<T, Size, Height> operator*(const Matrix<T, Size, Height>& m1, const DiagonalMatrix<T, Size>& m2) {
	return m1 * static_cast<Matrix<T, Size, Size>>(m2);
}


/*
	===== Other functions =====
*/

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height> outer(const Vector<T, Height>& v1, const Vector<T, Width>& v2) {
	Matrix<T, Width, Height> result;

	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width; col++) {
			result[row][col] = v1[row] * v2[col];
		}
	}

	return result;
}

template<typename T, size_t Size>
SymmetricMatrix<T, Size> selfOuter(const Vector<T, Size>& v) {
	SymmetricMatrix<T, Size> result;

	for (size_t row = 0; row < Size; row++) {
		for (size_t col = 0; col <= row; col++) {
			result[row][col] = v[row] * v[col];
		}
	}
	return result;
}
