#pragma once

#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"

class ScalableInertialMatrix {
	// not the exact diagonal components, they must still be added like: m[0][0] = diagonal.y + diagonal.z
	Vec3 diagonal; // scales (sx^3)*sy*sz, sx*(sy^3)*sz, sx*sy*(sz^3)
	Vec3 offDiagonal;
	// scales z = sx*sy*sz* sx*sy
	// scales y = sx*sy*sz* sx*sz
	// scales x = sx*sy*sz* sz*sy

public:
	ScalableInertialMatrix(Vec3 diagonalConstructors, Vec3 offDiagonal) : diagonal(diagonalConstructors), offDiagonal(offDiagonal) {}

	SymmetricMat3 toMatrix() const {
		return SymmetricMat3{
			diagonal.y + diagonal.z,
			offDiagonal.z, diagonal.x + diagonal.z,
			offDiagonal.y, offDiagonal.x, diagonal.x + diagonal.y
		};
	}

	SymmetricMat3 toMatrix(double scaleX, double scaleY, double scaleZ) const {
		double xyz = scaleX * scaleY * scaleZ;
		Vec3 scaledDC = xyz * elementWiseMul(diagonal, Vec3(scaleX * scaleX, scaleY * scaleY, scaleZ * scaleZ));
		Vec3 scaledOD = xyz * elementWiseMul(offDiagonal, Vec3(scaleY * scaleZ, scaleX * scaleZ, scaleX * scaleY));

		return SymmetricMat3{
			scaledDC.y + scaledDC.z,
			scaledOD.z, scaledDC.x + scaledDC.z,
			scaledOD.y, scaledOD.x, scaledDC.x + scaledDC.y
		};
	}

	SymmetricMat3 toMatrix(DiagonalMat3 scale) const {
		return toMatrix(scale[0], scale[1], scale[2]);
	}
};
