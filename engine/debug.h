#pragma once

#include "math/vec3.h"

namespace Debug {
	
	enum VecType {
		INFO,
		FORCE,
		MOMENT,
		IMPULSE,
		POSITION,
		VELOCITY,
		ANGULAR_VELOCITY,

	};

	void logVec(Vec3 origin, Vec3 vec, VecType type);

	void setVecLogAction(void(*logger)(Vec3 origin, Vec3 vec, VecType type));
}
