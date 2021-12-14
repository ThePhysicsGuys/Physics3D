#include "shapeClass.h"

namespace P3D {
ShapeClass::ShapeClass(double volume, Vec3 centerOfMass, ScalableInertialMatrix inertia, std::size_t intersectionClassID) :
	volume(volume),
	centerOfMass(centerOfMass),
	inertia(inertia),
	intersectionClassID(intersectionClassID),
	refCount(0) {}

ShapeClass::~ShapeClass() {}

double ShapeClass::getScaledMaxRadius(DiagonalMat3 scale) const {
	return sqrt(this->getScaledMaxRadiusSq(scale));
}

void ShapeClass::setScaleX(double newX, DiagonalMat3& scale) const {
	scale[0] = newX;
}
void ShapeClass::setScaleY(double newY, DiagonalMat3& scale) const {
	scale[1] = newY;
}
void ShapeClass::setScaleZ(double newZ, DiagonalMat3& scale) const {
	scale[2] = newZ;
}
};
