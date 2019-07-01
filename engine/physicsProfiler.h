#pragma once

#include "profiling.h"

enum class PhysicsProcess {
	TRANSFORMS,
	EXTERNALS,
	GJK_COL,
	GJK_NO_COL,
	EPA,
	COLISSION_OTHER,
	COLISSION_HANDLING,
	WAIT_FOR_LOCk,
	UPDATING,
	OTHER,
	COUNT
};

enum class IntersectionResult {
	COLISSION,
	GJK_REJECT,
	PHYSICAL_DISTANCE_REJECT,
	PHYSICAL_BOUNDS_REJECT,
	PART_DISTANCE_REJECT,
	PART_BOUNDS_REJECT,
	COUNT
};

enum class IterationTime {
	INSTANT_QUIT = 0,
	ONE_ITER = 1,
	TOOMANY = 15,
	LIMIT_REACHED = 16,
	COUNT = 17
};

extern BreakdownAverageProfiler<300, PhysicsProcess> physicsMeasure;
extern HistoricTally<300, long long, IntersectionResult> intersectionStatistics;
extern CircularBuffer<int, 100> gjkCollideIterStats;
extern CircularBuffer<int, 100> gjkNoCollideIterStats;
extern HistoricTally<100, long long, IterationTime> GJKCollidesIterationStatistics;
extern HistoricTally<100, long long, IterationTime> GJKNoCollidesIterationStatistics;
extern HistoricTally<100, long long, IterationTime> EPAIterationStatistics;
