#pragma once

#include "../engine/world.h"
#include "extendedPart.h"

class MagnetWorld : public World<ExtendedPart> {
public:
	Vec3 localSelectedPoint;
	Position magnetPoint;
	virtual void applyExternalForces() override;
};

class GravityWorld : public MagnetWorld {
private:
	Vec3 gravity;
public:
	GravityWorld(Vec3 gravity);

	virtual void applyExternalForces() override;
	virtual double getTotalPotentialEnergy() const override;
	virtual double getPotentialEnergyOfPhysical(const Physical& p) const override;
};
