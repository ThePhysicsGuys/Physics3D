#include "shapeClass.h"

ShapeClass::ShapeClass(double volume, Vec3 centerOfMass, ScalableInertialMatrix inertia, int intersectionClassID) : 
	volume(volume), 
	centerOfMass(centerOfMass), 
	inertia(inertia), 
	intersectionClassID(intersectionClassID) {}

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
