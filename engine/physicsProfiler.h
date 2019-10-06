#pragma once

#include "profiling.h"

enum class PhysicsProcess {
	GJK_COL,
	GJK_NO_COL,
	EPA,
	COLISSION_OTHER,
	EXTERNALS,
	COLISSION_HANDLING,
	CONSTRAINTS,
	UPDATE_TREE_BOUNDS,
	UPDATE_TREE_STRUCTURE,
	WAIT_FOR_LOCK,
	UPDATING,
	QUEUE,
	OTHER,
	COUNT
};

enum class IntersectionResult {
	COLISSION,
	GJK_REJECT,
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

extern BreakdownAverageProfiler<100, PhysicsProcess> physicsMeasure;
extern HistoricTally<1, long long, IntersectionResult> intersectionStatistics;
extern CircularBuffer<int, 1> gjkCollideIterStats;
extern CircularBuffer<int, 1> gjkNoCollideIterStats;
extern HistoricTally<1, long long, IterationTime> GJKCollidesIterationStatistics;
extern HistoricTally<1, long long, IterationTime> GJKNoCollidesIterationStatistics;
extern HistoricTally<1, long long, IterationTime> EPAIterationStatistics;
