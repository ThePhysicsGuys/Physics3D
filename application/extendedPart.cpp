#include "extendedPart.h"

ExtendedPart::ExtendedPart(const Shape& hitbox, const CFrame& position, double density, double friction, int drawMeshId, std::string name) : 
	Part(hitbox, position, density, friction), visualShape(hitbox) {
	this->name = name;
	this->drawMeshId = drawMeshId;
}
ExtendedPart::ExtendedPart(const VisualShape& shape, const CFrame& position, double density, double friction, int drawMeshId, std::string name) : 
	Part(static_cast<const VisualShape&>(shape), position, density, friction), visualShape(shape) {
	this->name = name;
	this->drawMeshId = drawMeshId;
}
ExtendedPart::ExtendedPart(const Shape& hitbox, const VisualShape& shape, const CFrame& position, double density, double friction, int drawMeshId, std::string name) :
	Part(hitbox, position, density, friction), visualShape(shape) {
	this->name = name;
	this->drawMeshId = drawMeshId;
}
