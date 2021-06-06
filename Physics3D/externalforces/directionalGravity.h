#pragma once

#include "../math/linalg/vec.h"
#include "externalForce.h"

namespace P3D {
class WorldPrototype;
class Part;
class MotorizedPhysical;

class DirectionalGravity : public ExternalForce {
public:
	Vec3 gravity;

	DirectionalGravity(Vec3 gravity) : gravity(gravity) {}

	virtual void apply(WorldPrototype* world) override;
	virtual double getPotentialEnergyForObject(const WorldPrototype* world, const Part& part) const override;
	virtual double getPotentialEnergyForObject(const WorldPrototype* world, const MotorizedPhysical& phys) const override;
};
};
