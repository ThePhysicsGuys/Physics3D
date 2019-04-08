#pragma once

#include "profiling.h"

enum class PhysicsProcess {
	TRANSFORMS,
	EXTERNALS,
	GJK,
	EPA,
	COLISSION_OTHER,
	COLISSION_HANDLING,
	UPDATING,
	OTHER,
	COUNT
};

enum class IntersectionResult {
	COLISSION,
	GJK_REJECT,
	DISTANCE_REJECT,
	COUNT
};

extern BreakdownAverageProfiler<300, PhysicsProcess> physicsMeasure;
extern HistoricTally<300, long long, IntersectionResult> intersectionStatistics;
