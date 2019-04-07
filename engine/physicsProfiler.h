#pragma once

#include "profiling.h"

enum class PhysicsProcess {
	TRANSFORMS,
	EXTERNALS,
	GJK,
	EPA,
	COLISSION_OTHER,
	UPDATING,
	OTHER,
	COUNT
};

extern BreakdownAverageProfiler<300, PhysicsProcess> physicsMeasure;
