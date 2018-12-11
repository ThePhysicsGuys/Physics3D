#pragma once

#include "gui/vectorMesh.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define glCall(x) clearError(); x; ASSERT(logCall(#x, __FILE__, __LINE__));

void clearError();

bool logCall(const char* func, const char* file, int line);

namespace AppDebug {
	void setupDebugHooks();

	void logTickStart();
	void logTickEnd();

	void updateVecMesh(VectorMesh* mesh);
}
