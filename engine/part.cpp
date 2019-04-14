#include "part.h"

// Part::Part() {}

Part::Part(NormalizedShape shape, CFrame position, double density, double friction) : hitbox(shape), cframe(position), properties({density, friction}) {
	this->maxRadius = hitbox.getMaxRadius();
}

Part::Part(Shape shape, CFrame position, double density, double friction) : properties({density, friction}) {
	CFrame backTransform;
	NormalizedShape normalized = shape.normalized(new Vec3[shape.vertexCount], backTransform);

	CFrame realCFrame = position.localToGlobal(backTransform);

	this->hitbox = normalized;
	this->cframe = realCFrame;
	this->maxRadius = hitbox.getMaxRadius();
}
