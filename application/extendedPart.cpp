#include "extendedPart.h"

ExtendedPart::ExtendedPart(NormalizedShape shape, CFrame position, double density, double friction, int drawMeshId) : Part(shape, position, density, friction) {
	this->drawMeshId = drawMeshId; 
}

ExtendedPart::ExtendedPart(Shape shape, CFrame position, double density, double friction, int drawMeshId) : Part(shape, position, density, friction) {
	this->drawMeshId = drawMeshId;
}
