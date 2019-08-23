#include "constraintGroup.h"

#include "math/largeMatrix.h"
#include "physical.h"

void ConstraintGroup::apply() const {
	size_t dimension = ballConstraints.size() * 3;
	LargeMatrix<double> systemToSolve(dimension, dimension);
	LargeVector<double> dragVector(dimension);
	LargeVector<double> velocityVector(dimension);
	LargeVector<double> accelerationVector(dimension);

	size_t matrixIndex = 0;

	for (const BallConstraint& bc : ballConstraints) {
		/*Local to A*/ Mat3 responseA = bc.a->getPointAccelerationMatrix(bc.attachA);
		/*Local to B*/ Mat3 responseB = bc.b->getPointAccelerationMatrix(bc.attachB);
		GlobalCFrame cfA = bc.a->getCFrame();
		GlobalCFrame cfB = bc.b->getCFrame();
		/*Global?*/ Mat3 selfResponse = cfA.rotation * responseA * cfA.rotation.inverse() + cfB.rotation * responseB * cfB.rotation.inverse();

		systemToSolve.setSubMatrix(matrixIndex, matrixIndex, selfResponse);

		matrixIndex += 3;
	}
	LargeMatrix<double> systemCopy(systemToSolve);
	LargeMatrix<double> systemCopyCopy(systemToSolve);

	// solve for position
	matrixIndex = 0;
	for (const BallConstraint& bc : ballConstraints) {
		Position posB = bc.b->getCFrame().localToGlobal(bc.attachB);
		Position posA = bc.a->getCFrame().localToGlobal(bc.attachA);
		dragVector.setSubVector(matrixIndex, Vec3(posB - posA));

		matrixIndex += 3;
	}
	destructiveSolve(systemCopyCopy, dragVector);

	matrixIndex = 0;
	for (const BallConstraint& bc : ballConstraints) {
		Vec3 drag;
		for (int i = 0; i < 3; i++) { drag[i] = dragVector[matrixIndex + i]; }
		bc.a->applyDrag(bc.a->getCFrame().localToRelative(bc.attachA), drag);
		bc.b->applyDrag(bc.b->getCFrame().localToRelative(bc.attachB), -drag);

		matrixIndex += 3;
	}

	// solve for velocity
	matrixIndex = 0;
	for (const BallConstraint& bc : ballConstraints) {
		Vec3 vb = bc.b->getVelocityOfPoint(bc.b->getCFrame().localToRelative(bc.attachB));
		Vec3 va = bc.a->getVelocityOfPoint(bc.a->getCFrame().localToRelative(bc.attachA));

		velocityVector.setSubVector(matrixIndex, vb - va);

		matrixIndex += 3;
	}
	destructiveSolve(systemToSolve, velocityVector);
	
	matrixIndex = 0;
	for (const BallConstraint& bc : ballConstraints) {
		Vec3 impulse;
		for (int i = 0; i < 3; i++) { impulse[i] = velocityVector[matrixIndex + i]; }
		bc.a->applyImpulse(bc.a->getCFrame().localToRelative(bc.attachA), impulse);
		bc.b->applyImpulse(bc.b->getCFrame().localToRelative(bc.attachB), -impulse);

		matrixIndex += 3;
	}

	// solve for acceleration
	matrixIndex = 0;
	for (const BallConstraint& bc : ballConstraints) {
		Vec3 ab = bc.b->getAccelerationOfPoint(bc.b->getCFrame().localToRelative(bc.attachB));
		Vec3 aa = bc.a->getAccelerationOfPoint(bc.a->getCFrame().localToRelative(bc.attachA));

		accelerationVector.setSubVector(matrixIndex, ab - aa);

		matrixIndex += 3;
	}

	destructiveSolve(systemCopy, accelerationVector);
	
	matrixIndex = 0;
	for (const BallConstraint& bc : ballConstraints) {
		Vec3 force;
		for (int i = 0; i < 3; i++) {force[i] = accelerationVector[matrixIndex + i];}
		bc.a->applyForce(bc.a->getCFrame().localToRelative(bc.attachA), force);
		bc.b->applyForce(bc.b->getCFrame().localToRelative(bc.attachB), -force);

		matrixIndex += 3;
	}
}
