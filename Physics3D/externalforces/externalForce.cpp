#include "externalForce.h"

#include "../world.h"
#include "../part.h"
#include "../physical.h"

namespace P3D {
double ExternalForce::getPotentialEnergyForObject(const WorldPrototype* world, const Part& part) const {
	return 0.0;
}
double ExternalForce::getPotentialEnergyForObject(const WorldPrototype* world, const MotorizedPhysical& phys) const {
	double total = 0.0;
	for(const Part& p : phys.rigidBody) {
		total += this->getPotentialEnergyForObject(world, p);
	}
	return total;
}

double ExternalForce::getTotalPotentialEnergyForThisForce(const WorldPrototype* world) const {
	double total = 0.0;
	for(MotorizedPhysical* p : world->physicals) {
		total += this->getPotentialEnergyForObject(world, *p);
	}
	return total;
}
};
