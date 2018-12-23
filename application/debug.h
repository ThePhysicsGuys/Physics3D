#pragma once

#include "../engine/math/vec3.h"
#include "buffers.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define glCall(x) clearError(); x; ASSERT(logCall(#x, __FILE__, __LINE__));

void clearError();

bool logCall(const char* func, const char* file, int line);


namespace AppDebug {
	struct ColoredVec {
		Vec3 origin, vec;
		double color;
		ColoredVec(Vec3 origin, Vec3 vec, double color) : origin(origin), vec(vec), color(color) {}
	};

	void setupDebugHooks();

	void logTickStart();
	void logTickEnd();

	AddableBuffer<ColoredVec> getVecBuffer();
}
