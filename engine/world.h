#pragma once

#include "part.h"

class World {
public:
	World();

	void tick(double deltaT);
	void addObject(Part p);
};
