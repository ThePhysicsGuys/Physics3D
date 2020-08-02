#include "testsMain.h"

#include "compare.h"
#include "testValues.h"
#include "simulation.h"
#include "randomValues.h"

#include <array>

#include "../physics/misc/toString.h"
#include "../physics/math/taylorExpansion.h"

#define DELTA_T 0.005

TEST_CASE(testEstimateDerivativesOfComputeOverTime) {
	FullTaylorExpansion<double, 5> startTaylor = createRandomFullTaylorExpansion<double, 5, createRandomDouble>();
	logStream << "startTaylor: " << startTaylor << "\n";
	std::array<double, 5> points = computeOverTime(startTaylor, DELTA_T);
	FullTaylorExpansion<double, 5> endTaylor = estimateDerivatives(points, DELTA_T);
	ASSERT_TOLERANT(startTaylor == endTaylor, 0.01);	
}
