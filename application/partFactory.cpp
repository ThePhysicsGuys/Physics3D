#include "partFactory.h"

PartFactory::PartFactory(const Shape& shape, Screen& screen, std::string name) {
	Vec3* normalBuf = (shape.normals) ? new Vec3[shape.vertexCount] : nullptr;
	this->normalizedShape = shape.normalized(new Vec3[shape.vertexCount], normalBuf, backTransform);
	this->drawMeshID = screen.addMeshShape(normalizedShape);
	this->name = name;
	this->count = 0;
}

PartFactory::PartFactory(const NormalizedShape& shape, Screen& screen, std::string name) : normalizedShape(shape) {
	this->drawMeshID = screen.addMeshShape(shape);
	this->name = name;
	this->count = 0;
}

ExtendedPart* PartFactory::produce(const CFrame& cframe, double density, double friction, std::string name) const {
	count++;
	CFrame realCFrame = cframe.localToGlobal(backTransform);
	return new ExtendedPart(normalizedShape, realCFrame, density, friction, drawMeshID, (name.empty()) ? this->name + " " + std::to_string(count) : name);
} 

ExtendedPart* createUniquePart(Screen& screen, const NormalizedShape& shape, CFrame position, double density, double friction, std::string name) {
	int id = screen.addMeshShape(shape);
	return new ExtendedPart(shape, position, density, friction, id, name);
}

ExtendedPart* createUniquePart(Screen& screen, const Shape& shape, CFrame position, double density, double friction, std::string name) {
	ExtendedPart* partPtr = new ExtendedPart(shape, position, density, friction, 0, name);
	int id = screen.addMeshShape(partPtr->hitbox);
	partPtr->drawMeshId = id;
	return partPtr;
}
