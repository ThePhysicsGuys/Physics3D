#include "extendedPart.h"

ExtendedPart::ExtendedPart(NormalizedShape shape, CFrame position, double density, double friction, int drawMeshId, std::string name) : Part(shape, position, density, friction) {
	this->name = name;
	this->drawMeshId = drawMeshId; 
}

ExtendedPart::ExtendedPart(Shape shape, CFrame position, double density, double friction, int drawMeshId, std::string name) : Part(shape, position, density, friction) {
	this->name = name;
	this->drawMeshId = drawMeshId;
}
