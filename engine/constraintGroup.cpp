#include "constraintGroup.h"

#include "math/largeMatrix.h"
#include "physical.h"

void ConstraintGroup::apply() const {
	size_t dimension = ballConstraints.size() * 3;
	LargeMatrix<double> systemToSolve(dimension, dimension);
	LargeVector<double> bVector(dimension);

	size_t matrixIndex = 0;

	for (const BallConstraint& bc : ballConstraints) {
		/*Local to A*/ Mat3 responseA = bc.a->getPointAccelerationMatrix(bc.attachA);
		/*Local to B*/ Mat3 responseB = bc.b->getPointAccelerationMatrix(bc.attachB);
		//throw "SelfResponse is not correct!!!!! I think it should be something like rotation * ";
		GlobalCFrame cfA = bc.a->getCFrame();
		GlobalCFrame cfB = bc.b->getCFrame();
		/*Global?*/ Mat3 selfResponse = cfA.rotation * responseA * cfA.rotation.inverse() + cfB.rotation * responseB * cfB.rotation.inverse();

		systemToSolve.setSubMatrix(matrixIndex, matrixIndex, selfResponse);

		Vec3 vb = bc.b->getVelocityOfPoint(bc.b->getCFrame().localToRelative(bc.attachB));
		Vec3 va = bc.a->getVelocityOfPoint(bc.a->getCFrame().localToRelative(bc.attachA));

		bVector.setSubVector(matrixIndex, vb - va);
		matrixIndex += 3;
	}

	destructiveSolve(systemToSolve, bVector);

	matrixIndex = 0;
	for (const BallConstraint& bc : ballConstraints) {
		Vec3 impulse;
		for (int i = 0; i < 3; i++) {
			impulse[i] = bVector[matrixIndex + i];
		}
		bc.a->applyImpulse(bc.a->getCFrame().localToRelative(bc.attachA), impulse);
		bc.b->applyImpulse(bc.b->getCFrame().localToRelative(bc.attachB), -impulse);

		matrixIndex += 3;
	}
}
