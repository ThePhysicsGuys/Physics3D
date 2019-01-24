#include "part.h"

// Part::Part() {}

Part::Part(NormalizedShape s, CFrame position, double density, double friction) : hitbox(s), cframe(position), properties({density, friction}) {}

Part::Part(Shape s, CFrame position, double density, double friction) : properties({density, friction}) {
	CFrame backTransform;
	NormalizedShape normalized = s.normalized(new Vec3[s.vCount], backTransform);

	CFrame realCFrame = position.localToGlobal(backTransform);

	this->hitbox = normalized;
	this->cframe = realCFrame;
}
