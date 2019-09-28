#pragma once

#include "../engine/synchonizedWorld.h"
#include "extendedPart.h"

class MagnetWorld : public SynchronizedWorld<ExtendedPart> {
public:
	MagnetWorld(double deltaT);
	Vec3 localSelectedPoint;
	Position magnetPoint;
	virtual void applyExternalForces() override;

	Part* selectedPart = nullptr;
};

class GravityWorld : public MagnetWorld {
private:
	Vec3 gravity;
public:
	GravityWorld(double deltaT, Vec3 gravity);

	virtual void applyExternalForces() override;
	virtual double getTotalPotentialEnergy() const override;
	virtual double getPotentialEnergyOfPhysical(const Physical& p) const override;
};
