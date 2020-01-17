#pragma once

#include "../physics/synchonizedWorld.h"
#include "extendedPart.h"
#include "../physics/math/position.h"

namespace Application {

class PlayerWorld : public SynchronizedWorld<ExtendedPart> {
public:
	PlayerWorld(double deltaT);
	Vec3 localSelectedPoint;
	Position magnetPoint;
	virtual void applyExternalForces() override;

	Part* selectedPart = nullptr;
};

};