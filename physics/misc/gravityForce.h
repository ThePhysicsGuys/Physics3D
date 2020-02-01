#pragma once

#include "../math/linalg/vec.h"
#include "../world.h"

class DirectionalGravity : public ExternalForce {
public:
	Vec3 gravity;

	DirectionalGravity(Vec3 gravity) : gravity(gravity) {}

	virtual void apply(WorldPrototype* world) override {
		for (MotorizedPhysical* p : world->iterPhysicals()) {
			p->applyForceAtCenterOfMass(gravity * p->totalMass);
		}
	}
	virtual double getPotentialEnergyForObject(const WorldPrototype* world, const Part& part) const override {
		return Vec3(Position() - part.getCenterOfMass()) * gravity * part.getMass();
	}
	virtual double getPotentialEnergyForObject(const WorldPrototype* world, const MotorizedPhysical& phys) const override {
		return Vec3(Position() - phys.getCenterOfMass()) * gravity * phys.totalMass;
	}
};
