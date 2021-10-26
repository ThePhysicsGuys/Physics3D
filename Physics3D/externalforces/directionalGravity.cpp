#include "directionalGravity.h"

#include "../math/position.h"
#include "../part.h"
#include "../physical.h"
#include "../world.h"

namespace P3D {
void DirectionalGravity::apply(WorldPrototype* world) {
	for(MotorizedPhysical* p : world->physicals) {
		p->applyForceAtCenterOfMass(gravity * p->totalMass);
	}
}
double DirectionalGravity::getPotentialEnergyForObject(const WorldPrototype* world, const Part& part) const {
	return Vec3(Position() - part.getCenterOfMass()) * gravity * part.getMass();
}
double DirectionalGravity::getPotentialEnergyForObject(const WorldPrototype* world, const MotorizedPhysical& phys) const {
	return Vec3(Position() - phys.getCenterOfMass()) * gravity * phys.totalMass;
}
};
