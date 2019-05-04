#include "partFactory.h"

PartFactory::PartFactory(const Shape& shape, Screen& screen) {
	Vec3* normalBuf = (shape.normals) ? new Vec3[shape.vertexCount] : nullptr;
	normalizedShape = shape.normalized(new Vec3[shape.vertexCount], normalBuf, backTransform);
	drawMeshID = screen.addMeshShape(normalizedShape);
}

PartFactory::PartFactory(const NormalizedShape& shape, Screen& screen) : normalizedShape(shape) {
	drawMeshID = screen.addMeshShape(shape);
}

ExtendedPart* PartFactory::produce(const CFrame& cframe, double density, double friction) const {
	CFrame realCFrame = cframe.localToGlobal(backTransform);
	return new ExtendedPart(normalizedShape, realCFrame, density, friction, drawMeshID);
}

ExtendedPart* createUniquePart(Screen& screen, const NormalizedShape& shape, CFrame position, double density, double friction) {
	int id = screen.addMeshShape(shape);
	return new ExtendedPart(shape, position, density, friction, id);
}

ExtendedPart* createUniquePart(Screen& screen, const Shape& shape, CFrame position, double density, double friction) {
	ExtendedPart* partPtr = new ExtendedPart(shape, position, density, friction, 0);
	int id = screen.addMeshShape(partPtr->hitbox);
	partPtr->drawMeshId = id;
	return partPtr;
}
