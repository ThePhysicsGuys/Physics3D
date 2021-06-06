#pragma once

#include "extendedPart.h"
#include <Physics3D/math/position.h>
#include <Physics3D/threading/synchonizedWorld.h>

namespace P3D::Application {

class PlayerWorld : public SynchronizedWorld<ExtendedPart> {
public:
	PlayerWorld(double deltaT);

	Part* selectedPart = nullptr;
	Vec3 localSelectedPoint;
	Position magnetPoint;
	
	void applyExternalForces() override;

	void onPartAdded(ExtendedPart* part) override;
	void onPartRemoved(ExtendedPart* part) override;
};

};