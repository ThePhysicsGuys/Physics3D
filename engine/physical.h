#pragma once

#include "math/mat3.h"
#include "math/vec3.h"
#include "part.h"


struct Physical {
	Part part;
	CFrame cframe;
	Vec3 velocity = Vec3();
	Physical(Part p, CFrame cframe);

	void update(double deltaT);
};
