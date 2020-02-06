#include "intersection.h"

#include "genericIntersection.h"
#include "../physicsProfiler.h"
#include "../profiling.h"
#include "computationBuffer.h"

#include "shape.h"
#include "polyhedron.h"

#include "../misc/validityHelper.h"
#include "shapeClass.h"

#include <algorithm>

std::optional<Intersection> intersectsTransformed(const Shape& first, const Shape& second, const CFrame& relativeTransform) {
	return intersectsTransformed(*first.baseShape, *second.baseShape, relativeTransform, first.scale, second.scale);
}


ComputationBuffers buffers(1000, 2000);

std::optional<Intersection> intersectsTransformed(const GenericCollidable& first, const GenericCollidable& second, const CFrame& relativeTransform, const DiagonalMat3& scaleFirst, const DiagonalMat3& scaleSecond) {
	ColissionPair info{first, second, relativeTransform, scaleFirst, scaleSecond};
	physicsMeasure.mark(PhysicsProcess::GJK_COL);
	std::optional collides = runGJKTransformed(info, -relativeTransform.position);

	if(collides) {
		Tetrahedron& result = collides.value();
		physicsMeasure.mark(PhysicsProcess::EPA);
		Vec3f intersection;
		Vec3f exitVector;

		if(!isfinite(result.A.p.x) || !isfinite(result.A.p.y) || !isfinite(result.A.p.z)) {
			intersection = Vec3f(0.0f, 0.0f, 0.0f);
			exitVector = Vec3f(std::min(scaleFirst[0], std::min(scaleFirst[1], std::min(scaleFirst[2], std::min(scaleSecond[0], std::min(scaleSecond[1], scaleSecond[2]))))),
							   0.0f, 0.0f);

			return Intersection(intersection, exitVector);
		}

		assert(isVecValid(result.A.p));
		assert(isVecValid(result.A.originFirst));
		assert(isVecValid(result.A.originSecond));
		assert(isVecValid(result.B.p));
		assert(isVecValid(result.B.originFirst));
		assert(isVecValid(result.B.originSecond));
		assert(isVecValid(result.C.p));
		assert(isVecValid(result.C.originFirst));
		assert(isVecValid(result.C.originSecond));
		assert(isVecValid(result.D.p));
		assert(isVecValid(result.D.originFirst));
		assert(isVecValid(result.D.originSecond));

		bool epaResult = runEPATransformed(info, result, intersection, exitVector, buffers);

		assert(isVecValid(exitVector));
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
