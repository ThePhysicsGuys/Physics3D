#pragma once

#include "../engine/world.h"

class GravityFloorWorld : public World {
	Vec3 gravity;
public:
	GravityFloorWorld(Vec3 gravity);

	Physical* selectedPhysical = nullptr;
	Vec3 relativeSelectedPoint;
	Vec3 absoluteSelectedPoint;
	Vec3 magnetPoint;

	virtual void applyExternalForces(const Shape* transformedShapes) override;
};
