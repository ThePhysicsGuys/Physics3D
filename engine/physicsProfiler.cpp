#include "physicsProfiler.h"

const char * labels[]{
	"Transforms",
	"Externals",
	"GJK",
	"EPA",
	"Collision",
	"Updates",
	"Other"
};
BreakdownAverageProfiler<300, PhysicsProcess> physicsMeasure(labels);
