#pragma once

#include "../engine/synchonizedWorld.h"
#include "extendedPart.h"

class PlayerWorld : public SynchronizedWorld<ExtendedPart> {
public:
	PlayerWorld(double deltaT);
	Vec3 localSelectedPoint;
	Position magnetPoint;
	virtual void applyExternalForces() override;

	Part* selectedPart = nullptr;
};
