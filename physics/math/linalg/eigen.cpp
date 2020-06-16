#include "eigen.h"

#include <cmath>

template<typename N>
N get(Matrix<N, 3, 3>& copy, int row, int col) {
	return copy[row][col];
}
template<typename N>
void set(Matrix<N, 3, 3>& copy, int row, int col, N value) {
	copy[row][col] = value;
}
template<typename N>
void update(EigenValues<N, 3>& e, bool* changed, int k, N t) {
	N y = e[k];
	e[k] = y + t;
	changed[k] = !(y == e[k]);
}
template<typename N>
void rotateEigen(Matrix<N, 3, 3>& copy, int k, int l, int i, int j, N c, N s) {
	N SKL = get(copy, k, l);
	N SIJ = get(copy, i, j);
	set(copy, k, l, c * SKL - s * SIJ);
	set(copy, i, j, s * SKL + c * SIJ);
}

EigenSet<double, 3> getEigenDecomposition(const SymmetricMat3& sm) {

	Mat3 copy(sm);

	EigenValues<double, 3> eigenValues{sm[0][0],sm[1][1],sm[2][2]};
	Mat3 eigenVectors = Mat3::IDENTITY();

	bool changed[3]{true, true, true};


	int tieBreaker = 0;
	const int values[6]{
		0,1,
		0,2,
		1,2
	};
	while(changed[0] || changed[1] || changed[2]) {
		double top = std::abs(get(copy, 0, 1));
		double topRight = std::abs(get(copy, 0, 2));
		double right = std::abs(get(copy, 1, 2));

		int k, l;

		// find which of the three upper off-diagonal elements is the biggest
		if(top > topRight && top > right) { k = 0; l = 1; } 
		else if(topRight > top && topRight > right) { k = 0; l = 2; } 
		else if(right > top && right > topRight) { k = 1; l = 2; } else {
			// TIEBREAKER
			k = values[tieBreaker * 2]; l = values[tieBreaker * 2 + 1];
			tieBreaker = (tieBreaker + 1) % 3;
		}

		double p = get(copy, k, l);

		if(p == 0) {
			return EigenSet<double, 3>(eigenValues, eigenVectors);
		}


		double y = (eigenValues[l] - eigenValues[k]) / 2; 
		double d = std::abs(y) + std::sqrt(p * p + y * y);
		double r = std::sqrt(p * p + d * d); 
		double c = d / r; 
		double s = p / r; 
		double t = p * p / d;

		if(y < 0) { s = -s; t = -t; };

		set(copy, k, l, 0.0); update(eigenValues, changed, k, -t); update(eigenValues, changed, l, t);

		for(int i = 0; i <= k - 1; i++) rotateEigen(copy, i, k, i, l, c, s);
		for(int i = k + 1; i <= l - 1; i++) rotateEigen(copy, k, i, i, l, c, s);
		for(int i = l + 1; i < 3; i++) rotateEigen(copy, k, i, l, i, c, s);

		for(int i = 0; i < 3; i++) {
			double EIK = get(eigenVectors, i, k);
			double EIL = get(eigenVectors, i, l);
			set(eigenVectors, i, k, c * EIK - s * EIL);
			set(eigenVectors, i, l, s * EIK + c * EIL);
		}
	}
	return EigenSet<double, 3>(eigenValues, eigenVectors);
}
