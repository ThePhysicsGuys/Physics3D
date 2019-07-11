#include "mat3.h"

#include <cmath>

template<typename N>
Mat3Template<N> rotX(N angle) {
	N sina = sin(angle);
	N cosa = cos(angle);
	return Mat3Template<N>
			   (1,  0,    0,
				0, cosa, sina,
				0,-sina, cosa);
}
template<typename N>
Mat3Template<N> rotY(N angle) {
	N sina = sin(angle);
	N cosa = cos(angle);
	return Mat3Template<N>
			   (cosa, 0,-sina,
				 0,   1,  0,
				sina, 0, cosa);
}
template<typename N>
Mat3Template<N> rotZ(N angle) {
	N sina = sin(angle);
	N cosa = cos(angle);
	return Mat3Template<N>
			   (cosa, sina, 0,
			   -sina, cosa, 0,
				0,     0,   1);
}
/*
	Produces a rotation matrix from the provided euler angles

	Equivalent to rotZ(gamma) * rotX(alpha) * rotY(beta)
*/
template<typename N>
Mat3Template<N> fromEulerAngles(N alpha, N beta, N gamma) {
	return rotZ(gamma) * rotX(alpha) * rotY(beta);
}

template<typename N>
Mat3Template<N> Mat3Template<N>::rotate(N angle, N x, N y, N z) const {
	N s = sin(angle);
	N c = cos(angle);
	N C = 1 - c;
	N rm00 = x * x * C + c;
	N rm01 = x * y * C + z * s;
	N rm02 = x * z * C - y * s;
	N rm10 = x * y * C - z * s;
	N rm11 = y * y * C + c;
	N rm12 = y * z * C + x * s;
	N rm20 = x * z * C + y * s;
	N rm21 = y * z * C - x * s;
	N rm22 = z * z * C + c;
	N r00 = m00 * rm00 + m10 * rm01 + m20 * rm02;
	N r01 = m01 * rm00 + m11 * rm01 + m21 * rm02;
	N r02 = m02 * rm00 + m12 * rm01 + m22 * rm02;
	N r10 = m00 * rm10 + m10 * rm11 + m20 * rm12;
	N r11 = m01 * rm10 + m11 * rm11 + m21 * rm12;
	N r12 = m02 * rm10 + m12 * rm11 + m22 * rm12;
	N r20 = m00 * rm20 + m10 * rm21 + m20 * rm22;
	N r21 = m01 * rm20 + m11 * rm21 + m21 * rm22;
	N r22 = m02 * rm20 + m12 * rm21 + m22 * rm22;

	return Mat3Template<N>(r00, r01, r02, r10, r11, r12, r20, r21, r22);
}

template<typename N>
Mat3Template<N> fromRotationVec(Vec3Template<N> rotVec) {
	
	N angle = rotVec.length();

	if (!(angle != 0)) { // inverse is important! Catches weird values like Nan and Inf too
		return Mat3Template<N>(1, 0, 0,
							   0, 1, 0,
							   0, 0, 1);
	}

	rotVec /= angle;
	N x = rotVec.x;
	N y = rotVec.y;
	N z = rotVec.z;

	N sinA = sin(angle);
	N cosA = cos(angle);

	Mat3Template<N> outerProd = outer(rotVec, rotVec);
	Mat3Template<N> rotor = Mat3Template<N>(cosA,     z*sinA,   -y*sinA,
											-z*sinA,  cosA,     x*sinA,
											y*sinA,   -x*sinA,  cosA);

	return outerProd * (1 - cosA) + rotor;
}

template<typename N>
Mat3Template<N> rotateAround(N angle, Vec3Template<N> normal) {
	// Using Rodrigues rotation formula;
	normal = normal.normalize();
	Mat3Template<N> W = Mat3Template<N> (
		 0,			normal.z, -normal.y,
		-normal.z,  0,		   normal.x,
		 normal.y, -normal.x,  0
	);

	Mat3Template<N> W2 = W * W;
	N s = sin(angle);
	N s2 = sin(angle / 2);

	Mat3Template<N> R = Mat3Template<N>() + W * s + W2 * (2 * s2 * s2);

	return R;
}

template<typename N>
N get(Mat3Template<N>& copy, int row, int col) {
	return copy.m[row * 3 + col];
}
template<typename N>
void set(Mat3Template<N>& copy, int row, int col, N value) {
	copy.m[row * 3 + col] = value;
}
template<typename N>
void update(EigenValues<N>& e, bool* changed, int k, N t) {
	N y = e[k];
	e[k] = y + t;
	changed[k] = !(y == e[k]);
}
template<typename N>
void rotateEigen(Mat3Template<N>& copy, int k, int l, int i, int j, N c, N s) {
	N SKL = get(copy, k, l);
	N SIJ = get(copy, i, j);
	set(copy, k, l, c*SKL - s*SIJ);
	set(copy, i, j, s*SKL + c*SIJ);
}

/*
	Calculates the eigenvalues and eigenvectors of a symmetric matrix using the Jacobi Eigenvalue Algorythm

	It works by rotating the given matrix until it becomes a diagonal matrix
*/

template<typename N>
EigenSet<N> SymmetricMat3Template<N>::getEigenDecomposition() const {
	
	Mat3Template<N> copy(m00, m01, m02, m10, m11, m12, m20, m21, m22);

	EigenValues<N> eigenValues(m00, m11, m22);
	Mat3Template<N> eigenVectors(1,0,0,0,1,0,0,0,1);
	
	bool changed[3]{true, true, true};


	int tieBreaker = 0;
	const int values[6] {
		0,1,
		0,2,
		1,2
	};
	while(changed[0] || changed[1] || changed[2]) {
		N top = abs(get(copy, 0, 1));
		N topRight = abs(get(copy, 0, 2));
		N right = abs(get(copy, 1, 2));
		
		int k, l;
		
		// find which of the three upper off-diagonal elements is the biggest
		if (top > topRight && top > right) { k = 0; l = 1; }
		else if (topRight > top && topRight > right) { k = 0; l = 2; }
		else if (right > top && right > topRight) { k = 1; l = 2; }
		else {
			// TIEBREAKER
			k = values[tieBreaker *2]; l = values[tieBreaker *2+1];
			tieBreaker = (tieBreaker + 1) % 3;
		}

		N p = get(copy, k, l);

		if(p == 0) {
			return EigenSet<N>(eigenValues, eigenVectors.transpose());
		}


		N y = (eigenValues[l] - eigenValues[k]) / 2; N d = abs(y) + sqrt(p*p + y*y);
		N r = sqrt(p*p + d*d); N c = d / r; N s = p / r; N t = p*p / d;

		if(y < 0) { s = -s; t = -t; };

		set(copy, k, l, (N) 0); update(eigenValues, changed, k, -t); update(eigenValues, changed, l, t);

		for(int i = 0; i <= k - 1; i++) rotateEigen(copy, i, k, i, l, c, s);
		for(int i = k + 1; i <= l - 1; i++) rotateEigen(copy, k, i, i, l, c, s);
		for(int i = l + 1; i < 3; i++) rotateEigen(copy, k, i, l, i, c, s);

		for(int i = 0; i < 3; i++) {
			N EIK = get(eigenVectors, i, k);
			N EIL = get(eigenVectors, i, l);
			set(eigenVectors, i, k, c*EIK - s*EIL);
			set(eigenVectors, i, l, s*EIK + c*EIL);
		}
	}
	return EigenSet<N>(eigenValues, eigenVectors.transpose());
}

template<typename N>
EigenSet<N> DiagonalMat3Template<N>::getEigenDecomposition() const {
	return EigenSet<N>(EigenValues<N>(m00, m11, m22), Mat3Template<N>());
}

template struct Mat3Template<double>;
template struct Mat3Template<float>;
template struct SymmetricMat3Template<double>;
template struct SymmetricMat3Template<float>;
template struct DiagonalMat3Template<double>;
template struct DiagonalMat3Template<float>;

template Mat3Template<double> rotX(double);
template Mat3Template<double> rotY(double);
template Mat3Template<double> rotZ(double);
template Mat3Template<double> fromEulerAngles(double, double, double);
template Mat3Template<float> fromEulerAngles(float, float, float);
template Mat3Template<double> fromRotationVec(Vec3Template<double> v);
template Mat3Template<float> fromRotationVec(Vec3Template<float> v);
template Mat3Template<double> rotateAround(double angle, Vec3Template<double> normal);
template Mat3Template<float> rotateAround(float angle, Vec3Template<float> normal);
