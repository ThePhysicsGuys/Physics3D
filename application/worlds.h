#pragma once

#include "extendedPart.h"
#include "../engine/ecs/tree.h"
#include "../physics/math/position.h"
#include "../physics/synchonizedWorld.h"

namespace P3D::Application {

class PlayerWorld : public SynchronizedWorld<ExtendedPart> {
public:
	PlayerWorld(double deltaT);

	Part* selectedPart = nullptr;
	Vec3 localSelectedPoint;
	Position magnetPoint;
	
	virtual void applyExternalForces() override;
	virtual void onPartAdded(ExtendedPart* part) override;
	virtual void onPartRemoved(ExtendedPart* part) override;
};

};