#pragma once

#include "../engine/world.h"

class GravityFloorWorld : public World {
private:
	Vec3 gravity;
public:
	GravityFloorWorld(Vec3 gravity);

	virtual void applyExternalForces() override;
};
