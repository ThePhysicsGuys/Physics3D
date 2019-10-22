#include "intersection.h"

#include "genericIntersection.h"
#include "../physicsProfiler.h"
#include "../profiling.h"
#include "computationBuffer.h"

#include "shape.h"
#include "polyhedron.h"



Intersection intersectsTransformed(const Shape& first, const Shape& second, const CFrame& relativeTransform) {
	return intersectsTransformed(*first.polyhedron, *second.polyhedron, relativeTransform);
}


void incDebugTally(HistoricTally<long long, IterationTime>& tally, int iterTime) {
	if(iterTime >= 200) {
		tally.addToTally(IterationTime::LIMIT_REACHED, 1);
	} else if(iterTime >= 15) {
		tally.addToTally(IterationTime::TOOMANY, 1);
	} else {
		tally.addToTally(static_cast<IterationTime>(iterTime), 1);
	}
}


ComputationBuffers buffers(1000, 2000);

Intersection intersectsTransformed(const GenericCollidable& first, const GenericCollidable& second, const CFrame& relativeTransform) {
	Tetrahedron result;
	int iter;
	physicsMeasure.mark(PhysicsProcess::GJK_COL);
	bool collides = runGJKTransformed(first, second, relativeTransform, -relativeTransform.position, result, iter);

	if(collides) {
		incDebugTally(GJKCollidesIterationStatistics, iter + 2);
		physicsMeasure.mark(PhysicsProcess::EPA);
		Vec3f intersection;
		Vec3f exitVector;
		bool epaResult = runEPATransformed(first, second, result, relativeTransform, intersection, exitVector, buffers, iter);
		incDebugTally(EPAIterationStatistics, iter);
		if(!epaResult) {
			return Intersection();
		} else {
			return Intersection(intersection, exitVector);
		}
	} else {
		incDebugTally(GJKNoCollidesIterationStatistics, iter + 2);
		physicsMeasure.mark(PhysicsProcess::OTHER, PhysicsProcess::GJK_NO_COL);
		return Intersection();
	}
}
