#pragma once

#include "../engine/world.h"
#include "extendedPart.h"

class GravityWorld : public World<ExtendedPart> {
private:
	Vec3 gravity;
public:
	GravityWorld(Vec3 gravity);

	virtual void applyExternalForces() override;
};
