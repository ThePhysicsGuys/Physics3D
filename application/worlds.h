#pragma once

#include "../engine/world.h"

class GravityFloorWorld : public World {
	Vec3 gravity;
public:
	GravityFloorWorld(Vec3 gravity);
	virtual void applyExternalForces(const Shape* transformedShapes) override;
};
