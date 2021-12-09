#pragma once

#include "extendedPart.h"
#include <Physics3D/math/position.h>
#include <Physics3D/world.h>

namespace P3D::Application {

class PlayerWorld : public World<ExtendedPart> {
public:
	PlayerWorld(double deltaT);

	void onPartAdded(ExtendedPart* part) override;
	void onPartRemoved(ExtendedPart* part) override;
};

};