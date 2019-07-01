#include "physicsProfiler.h"

const char * physicsLabels[]{
	"Transforms",
	"Externals",
	"GJK Col",
	"GJK No Col",
	"EPA",
	"Collision",
	"Col. Handling",
	"Wait for lock",
	"Updates",
	"Other"
};

const char * intersectionLabels[]{
	"Colission",
	"GJK reject",
	"Phys Dist reject",
	"Phys Bound reject",
	"Part Dist reject",
	"Part Bound reject"
};

const char* iterationLabels[]{
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"10",
	"11",
	"12",
	"13",
	"14",
	"15+",
	"MAX",
};

BreakdownAverageProfiler<300, PhysicsProcess> physicsMeasure(physicsLabels);
HistoricTally<300, long long, IntersectionResult> intersectionStatistics(intersectionLabels);
CircularBuffer<int, 100> gjkCollideIterStats;
CircularBuffer<int, 100> gjkNoCollideIterStats;

HistoricTally<100, long long, IterationTime> GJKCollidesIterationStatistics(iterationLabels);
HistoricTally<100, long long, IterationTime> GJKNoCollidesIterationStatistics(iterationLabels);
HistoricTally<100, long long, IterationTime> EPAIterationStatistics(iterationLabels);
