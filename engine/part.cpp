#include "part.h"

Part::Part(NormalizedShape shape, CFrame position, double density, double friction) : hitbox(shape), cframe(position), properties({density, friction}) {
	this->maxRadius = hitbox.getMaxRadius();
}

Part::Part(Shape shape, CFrame position, double density, double friction) : properties({density, friction}) {
	CFrame backTransform;
	Vec3* normalBuf = (shape.normals) ? new Vec3[shape.vertexCount] : nullptr;
	NormalizedShape normalized = shape.normalized(new Vec3[shape.vertexCount], normalBuf, backTransform);

	CFrame realCFrame = position.localToGlobal(backTransform);

	this->hitbox = normalized;
	this->cframe = realCFrame;
	this->maxRadius = hitbox.getMaxRadius();
}

bool Part::intersects(const Part& other, Vec3& intersection, Vec3& exitVector) const {
#ifdef USE_TRANSFORMATIONS
	return this->transformed.intersects(other.transformed, intersection, exitVector, other.cframe.position - this->cframe.position);
#else
	const CFrame relativeCFrame(this->cframe.globalToLocal(other.cframe));
	if(this->hitbox.intersectsTransformed(other.hitbox, relativeCFrame, intersection, exitVector)) {
		intersection = this->cframe.localToGlobal(intersection);
		exitVector = this->cframe.localToRelative(exitVector);
		return true;
	}
	return false;
#endif
}
