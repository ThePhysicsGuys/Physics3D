#pragma once

namespace P3D {
class WorldPrototype;
class Part;
class MotorizedPhysical;

class ExternalForce {
public:
	virtual void apply(WorldPrototype* world) = 0;

	// These do not necessarity have to be implemented. Used for world potential energy computation
	virtual double getPotentialEnergyForObject(const WorldPrototype* world, const Part& part) const;

	// default implementation sums potential energies of constituent parts
	virtual double getPotentialEnergyForObject(const WorldPrototype* world, const MotorizedPhysical& phys) const;
	virtual double getTotalPotentialEnergyForThisForce(const WorldPrototype* world) const;
};
};
