#include "core.h"

#include "extendedPart.h"

ExtendedPart::ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, double density, double friction, int drawMeshId, std::string name) : 
	Part(hitbox, position, density, friction), visualShape(hitbox) {
	this->name = name;
	this->drawMeshId = drawMeshId;
}
ExtendedPart::ExtendedPart(const VisualShape& shape, const GlobalCFrame& position, double density, double friction, int drawMeshId, std::string name) :
	Part(static_cast<const VisualShape&>(shape), position, density, friction), visualShape(shape) {
	this->name = name;
	this->drawMeshId = drawMeshId;
}
ExtendedPart::ExtendedPart(const Shape& hitbox, const VisualShape& shape, const GlobalCFrame& position, double density, double friction, int drawMeshId, std::string name) :
	Part(hitbox, position, density, friction), visualShape(shape) {
	this->name = name;
	this->drawMeshId = drawMeshId;
}

void ExtendedPart::scale(double scaleX, double scaleY, double scaleZ) {
	Part::scale(scaleX, scaleY, scaleZ);

	visualScale = DiagonalMat3f(float(scaleX), float(scaleY), float(scaleZ)) * visualScale;
}