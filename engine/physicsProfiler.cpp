#include "physicsProfiler.h"

const char * physicsLabels[]{
	"Transforms",
	"Externals",
	"GJK",
	"EPA",
	"Collision",
	"Col. Handling",
	"Updates",
	"Other"
};

const char * intersectionLabels[]{
	"Colission",
	"GJK reject",
	"Distance reject"
};

BreakdownAverageProfiler<300, PhysicsProcess> physicsMeasure(physicsLabels);
HistoricTally<300, long long, IntersectionResult> intersectionStatistics(intersectionLabels);
CircularBuffer<int, 100> gjkIterStats;
