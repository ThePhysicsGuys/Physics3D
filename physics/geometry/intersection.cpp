#include "intersection.h"

#include "genericIntersection.h"
#include "../misc/physicsProfiler.h"
#include "../misc/profiling.h"
#include "computationBuffer.h"

#include "shape.h"
#include "polyhedron.h"

#include "../misc/validityHelper.h"
#include "shapeClass.h"

#include "../catchable_assert.h"

#include <algorithm>

namespace P3D {
std::optional<Intersection> intersectsTransformed(const Shape& first, const Shape& second, const CFrame& relativeTransform) {
	return intersectsTransformed(*first.baseShape, *second.baseShape, relativeTransform, first.scale, second.scale);
}

thread_local ComputationBuffers buffers(1000, 2000);

std::optional<Intersection> intersectsTransformed(const GenericCollidable& first, const GenericCollidable& second, const CFrame& relativeTransform, const DiagonalMat3& scaleFirst, const DiagonalMat3& scaleSecond) {
	ColissionPair info{first, second, relativeTransform, scaleFirst, scaleSecond};
	physicsMeasure.mark(PhysicsProcess::GJK_COL);
	std::optional collides = runGJKTransformed(info, -relativeTransform.position);

	if(collides) {
		Tetrahedron& result = collides.value();
		physicsMeasure.mark(PhysicsProcess::EPA);
		Vec3f intersection;
		Vec3f exitVector;

		if(!std::isfinite(result.A.p.x) || !std::isfinite(result.A.p.y) || !std::isfinite(result.A.p.z)) {
			intersection = Vec3f(0.0f, 0.0f, 0.0f);
			float minOfScaleFirst = float(std::min(scaleFirst[0], std::min(scaleFirst[1], scaleFirst[2])));
			float minOfScaleSecond = float(std::min(scaleSecond[0], std::min(scaleSecond[1], scaleSecond[2])));
			exitVector = Vec3f(std::min(minOfScaleFirst, minOfScaleSecond), 0.0f, 0.0f);

			return Intersection(intersection, exitVector);
		}

		catchable_assert(isVecValid(result.A.p));
		catchable_assert(isVecValid(result.A.originFirst));
		catchable_assert(isVecValid(result.A.originSecond));
		catchable_assert(isVecValid(result.B.p));
		catchable_assert(isVecValid(result.B.originFirst));
		catchable_assert(isVecValid(result.B.originSecond));
		catchable_assert(isVecValid(result.C.p));
		catchable_assert(isVecValid(result.C.originFirst));
		catchable_assert(isVecValid(result.C.originSecond));
		catchable_assert(isVecValid(result.D.p));
		catchable_assert(isVecValid(result.D.originFirst));
		catchable_assert(isVecValid(result.D.originSecond));

		bool epaResult = runEPATransformed(info, result, intersection, exitVector, buffers);

		catchable_assert(isVecValid(exitVector));
		if(!epaResult) {
			return std::optional<Intersection>();
		} else {
			return std::optional<Intersection>(Intersection(intersection, exitVector));
		}
	} else {
		physicsMeasure.mark(PhysicsProcess::OTHER, PhysicsProcess::GJK_NO_COL);
		return std::optional<Intersection>();
	}
}
};
