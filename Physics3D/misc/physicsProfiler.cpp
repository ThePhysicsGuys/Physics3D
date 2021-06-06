#include "physicsProfiler.h"

namespace P3D {
const char * physicsLabels[]{
	"GJK Col",
	"GJK No Col",
	"EPA",
	"Collision",
	"Externals",
	"Col. Handling",
	"Constraints",
	"Tree Bounds",
	"Tree Structure",
	"Wait for lock",
	"Updates",
	"Queue",
	"Other"
};

const char * intersectionLabels[]{
	"Colission",
	"GJK Reject",
	"Part Dist Reject",
	"Part Bound Reject"
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

BreakdownAverageProfiler<PhysicsProcess> physicsMeasure(physicsLabels, 100);
HistoricTally<long long, IntersectionResult> intersectionStatistics(intersectionLabels, 1);
CircularBuffer<int> gjkCollideIterStats(1);
CircularBuffer<int> gjkNoCollideIterStats(1);

HistoricTally<long long, IterationTime> GJKCollidesIterationStatistics(iterationLabels, 1);
HistoricTally<long long, IterationTime> GJKNoCollidesIterationStatistics(iterationLabels, 1);
HistoricTally<long long, IterationTime> EPAIterationStatistics(iterationLabels, 1);
};