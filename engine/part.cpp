#include "part.h"

Part::Part(const Shape& shape, const GlobalCFrame& position, double density, double friction) : hitbox(shape), cframe(position), properties({density, friction}) {
	this->maxRadius = hitbox.getMaxRadius();
	this->localBounds = hitbox.getBounds();
	this->mass = this->hitbox.getVolume() * this->properties.density;
	this->inertia = this->hitbox.getInertia() * this->properties.density;
	this->localCenterOfMass = this->hitbox.getCenterOfMass();
}

bool Part::intersects(const Part& other, Position& intersection, Vec3& exitVector) const {
#ifdef USE_TRANSFORMATIONS
	Vec3f intersectionF, exitVectorF;
	if(this->transformed.intersects(other.transformed, intersectionF, exitVectorF, other.cframe.position - this->cframe.position)){
		intersection = Vec3(intersectionF);
		exitVector = Vec3(exitVectorF);
		return true;
	}
	return false;
#else
	const CFramef relativeCFrame(this->cframe.globalToLocal(other.cframe));
	Vec3f localIntersection, localExitVector;
	if(this->hitbox.intersectsTransformed(other.hitbox, relativeCFrame, localIntersection, localExitVector)) {
		intersection = this->cframe.localToGlobal(Vec3(localIntersection));
		exitVector = this->cframe.localToRelative(Vec3(localExitVector));
		return true;
	}
	return false;
#endif
}
