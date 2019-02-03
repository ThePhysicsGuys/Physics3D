#pragma once

#include "math/vec3.h"
#include "math/cframe.h"

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

	enum CFrameType {
		OBJECT_CFRAME,
		INERTIAL_CFRAME
	};

	void logVec(Vec3 origin, Vec3 vec, VecType type);
	void logCFrame(CFrame frame, CFrameType type);

	void setVecLogAction(void(*logger)(Vec3 origin, Vec3 vec, VecType type));
	void setCFrameLogAction(void(*logger)(CFrame frame, CFrameType type));
}
