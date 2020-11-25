#pragma once

#include <string>
#include <sstream>
#include <stddef.h>
#include <iomanip>

#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/linalg/eigen.h"
#include "../math/linalg/largeMatrix.h"
#include "../math/cframe.h"
#include "../math/position.h"
#include "../math/globalCFrame.h"
#include "../math/taylorExpansion.h"
#include "../motion.h"
#include "../relativeMotion.h"

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const UnmanagedLargeMatrix<T>& matrix) {
	for(int i = 0; i < matrix.h; i++) {
		for(int j = 0; j < matrix.w; j++) {
			os << matrix(i, j) << '\t';
		}
		os << '\n';
	}

	return os;
}

template<typename T, size_t Rows>
inline std::ostream& operator<<(std::ostream& os, const UnmanagedVerticalFixedMatrix<T, Rows>& matrix) {
	for(int i = 0; i < Rows; i++) {
		for(int j = 0; j < matrix.cols; j++) {
			os << matrix(i, j) << '\t';
		}
		os << '\n';
	}

	return os;
}

template<typename T, size_t Cols>
inline std::ostream& operator<<(std::ostream& os, const UnmanagedHorizontalFixedMatrix<T, Cols>& matrix) {
	for(int i = 0; i < matrix.rows; i++) {
		for(int j = 0; j < Cols; j++) {
			os << matrix(i, j) << '\t';
		}
		os << '\n';
	}

	return os;
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const UnmanagedLargeVector<T>& vector) {
	for(int i = 0; i < vector.n; i++) {
		os << vector[i] << ',';
	}

	return os;
}

template<typename T, size_t Size>
inline std::ostream& operator<<(std::ostream& os, const Vector<T, Size>& vector) {
	os << std::fixed << std::setprecision(4);
	os << '(';
	for(size_t i = 0; i < Size - 1; i++) {
		if (vector[i] >= 0)
			os << "+";
		os << vector[i] << ", ";
	}

	if (vector[Size - 1] >= 0)
		os << "+";
	
	os << vector[Size - 1] << ")";
	return os;
}

template<int64_t N>
inline std::ostream& operator<<(std::ostream& os, Fix<N> f) {
	int64_t intPart = f.value >> N;
	int64_t frac = f.value & ((1ULL << N) - 1);

	if(intPart < 0) {
		if(frac == 0) {
			intPart = -intPart;
		} else {
			intPart = -intPart-1;
			frac = (1ULL << N) - frac;
		}
		os << '-';
	}
	std::streamsize prec = os.precision();
	
	for(int i = 0; i < prec; i++) {
		frac *= 5; // multiply by powers of 10, dividing out the /2 with the shifts
		// multiply by 5 instead of 10, to stay away from the integer limit just a little longer
	}

	frac = frac >> (N - prec - 1); // shift right, but minus the divisions previously done, except for last bit, which is used for rounding
	if(frac & 1) {
		frac = (frac >> 1) + 1;

		int64_t maxFrac = 1;
		for(int i = 0; i < prec; i++) {
			maxFrac *= 10;
		}
		if(frac >= maxFrac) {
			frac = 0;
			++intPart;
		}
	} else {
		frac = frac >> 1;
	}

	bool fixed = os.flags() & std::ios::fixed;

	os << intPart;
	if(frac == 0) {
		if(fixed) {
			os << '.';
			for(int i = 0; i < prec; i++) {
				os << '0';
			}
		}
	} else {
		os << '.';
		std::streamsize digits = 1;
		int64_t digitCounter = 10;
		while(frac >= digitCounter) {
			digitCounter *= 10;
			digits++;
		}
		std::streamsize leadingZeros = prec - digits;
		for(int i = 0; i < leadingZeros; i++) {
			os << '0';
		}
		if(!fixed) {
			while(frac % 10 == 0) {
				frac /= 10;
			}
		}
		os << frac;
	}

	return os;
}

inline std::ostream& operator<<(std::ostream& os, Position position) {
	os << "(" << position.x << ", " << position.y << ", " << position.z << ")";
	return os;
}

template<typename N, size_t Width, size_t Height>
inline std::ostream& operator<<(std::ostream& os, const Matrix<N, Height, Width>& matrix) {
	os << "(";

	for(size_t row = 0; row < Height; row++) {
		for(size_t col = 0; col < Width - 1; col++) {
			os << matrix(row, col) << ", ";
		}
		os << matrix(row, Width - 1) << "; ";
	}
	os << ")";

	return os;
}

template<typename N, size_t Size>
inline std::ostream& operator<<(std::ostream& os, const SymmetricMatrix<N, Size>& matrix) {
	os << "(";

	for(size_t row = 0; row < Size; row++) {
		for(size_t col = 0; col < Size - 1; col++) {
			os << matrix(row, col) << ", ";
		}
		os << matrix(row, Size - 1) << "; ";
	}
	os << ")";

	return os;
}

template<typename N, size_t Size>
inline std::ostream& operator<<(std::ostream& os, const DiagonalMatrix<N, Size>& matrix) {
	os << "Diag(";

	for(size_t i = 0; i < Size; i++) {
		os << matrix[i] << "; ";
	}
	os << ")";

	return os;
}

template<typename N, size_t Width, size_t Height>
inline std::string str(const Matrix<N, Height, Width>& matrix) {
	std::stringstream ss;
	ss.precision(4);
	ss << matrix;
	return ss.str();
}
template<typename N, size_t Size>
inline std::string str(const SymmetricMatrix<N, Size>& matrix) {
	std::stringstream ss;
	ss.precision(4);
	ss << matrix;
	return ss.str();
}

template<typename N, size_t Size>
inline std::ostream& operator<<(std::ostream& os, const EigenValues<N, Size>& v) {
	os << "EigenValues(";
	for(size_t i = 0; i < Size - 1; i++)
		os << v[i] << ", ";

	os << v[Size - 1] << ")";
	return os;
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const Quaternion<T>& quat) {
	os << quat.w;
	if(quat.i >= 0) os << '+';
	os << quat.i << 'i';
	if(quat.j >= 0) os << '+';
	os << quat.j << 'j';
	if(quat.k >= 0) os << '+';
	os << quat.k << 'k';
	return os;
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const RotationTemplate<T>& rotation) {
	os << rotation.asRotationMatrix();
	return os;
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const CFrameTemplate<T>& cframe) {
	os << "CFrame(" << cframe.position << ", " << cframe.rotation << ")";
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const GlobalCFrame& cframe) {
	os << "GlobalCFrame(" << cframe.position << ", " << cframe.rotation << ")";
	return os;
}

template<typename T, std::size_t Size>
inline std::ostream& operator<<(std::ostream& os, const TaylorExpansion<T, Size>& taylor) {
	if constexpr(Size > 0) {
		os << taylor[0] << "x";
		for(std::size_t i = 1; i < Size; i++) {
			os << " + " << taylor[i] << "x^" << (i + 1) << '/' << (i+1) << '!';
		}
	}
	return os;
}

template<typename T, std::size_t Size>
inline std::ostream& operator<<(std::ostream& os, const FullTaylorExpansion<T, Size>& taylor) {
	os << taylor.getConstantValue() << " + ";
	os << taylor.getDerivatives();
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const TranslationalMotion& motion) {
	os << "{vel: " << motion.getVelocity();
	os << ", accel: " << motion.getAcceleration() << "}";
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const RotationalMotion& motion) {
	os << "{angularVel: " << motion.getAngularVelocity();
	os << ", angularAccel: " << motion.getAngularAcceleration() << "}";
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Motion& motion) {
	os << "{vel: " << motion.getVelocity();
	os << ", angularVel: " << motion.getAngularVelocity();
	os << ", accel: " << motion.getAcceleration();
	os << ", angularAccel: " << motion.getAngularAcceleration() << "}";

	return os;
}

inline std::ostream& operator<<(std::ostream& os, const RelativeMotion& relMotion) {
	os << "{motion: " << relMotion.relativeMotion;
	os << ", offset: " << relMotion.locationOfRelativeMotion << "}";

	return os;
}

template<typename T>
inline std::string str(const T& obj) {
	std::stringstream ss;
	ss.precision(4);
	ss << obj;
	return ss.str();
}
