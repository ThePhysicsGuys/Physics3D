#include "constraintGroup.h"

#include "math/linalg/largeMatrix.h"
#include "physical.h"
#include "math/linalg/mat.h"

#include "math/mathUtil.h"
#include <fstream>

LargeMatrix<double> computeInteractionMatrix(const ConstraintGroup& group) {
	const std::vector<BallConstraint>& ballConstraints = group.ballConstraints;

	size_t dimension = ballConstraints.size() * 3;
	LargeMatrix<double> systemToSolve(dimension, dimension);
	for (size_t i = 0; i < dimension; i++) {
		for (size_t j = 0; j < dimension; j++) {
			systemToSolve.get(i, j) = 0.0;
		}
	}

	size_t matrixIndex = 0;
	for (const BallConstraint& bc : ballConstraints) {
		/*Local to A*/ SymmetricMat3 responseA = bc.a->mainPhysical->getResponseMatrix(bc.a->localToMain(bc.attachA));
		/*Local to B*/ SymmetricMat3 responseB = bc.b->mainPhysical->getResponseMatrix(bc.a->localToMain(bc.attachB));
		GlobalCFrame cfA = bc.a->mainPhysical->getCFrame();
		GlobalCFrame cfB = bc.b->mainPhysical->getCFrame();
		/*Global?*/ SymmetricMat3 selfResponse = cfA.rotation.localToGlobal(responseA) + cfB.rotation.localToGlobal(responseB);

		systemToSolve.setSubMatrix(matrixIndex, matrixIndex, Mat3(selfResponse));

		matrixIndex += 3;
	}

	for (size_t i = 0; i < ballConstraints.size(); i++) {
		const BallConstraint& y = ballConstraints[i];

		for (size_t j = 0; j < ballConstraints.size(); j++) {
			if (i == j) continue;
			const BallConstraint& x = ballConstraints[j];

			// find effect of y constraint on velocities of x;

			bool isPositive;
			Physical* sharedBody;
			Vec3 actorOffset;
			Vec3 responseOffset;

				 if (x.a == y.a) { 
					 isPositive = true;  sharedBody = x.a; actorOffset = x.attachA; responseOffset = y.attachA; }
			else if (x.a == y.b) { 
					 isPositive = false; sharedBody = x.a; actorOffset = x.attachA; responseOffset = y.attachB; }
			else if (x.b == y.a) { 
					 isPositive = false; sharedBody = x.b; actorOffset = x.attachB; responseOffset = y.attachA; }
			else if (x.b == y.b) { 
					 isPositive = true;  sharedBody = x.b; actorOffset = x.attachB; responseOffset = y.attachB; }
			else {continue;}
			
			Mat3 response = sharedBody->mainPhysical->getResponseMatrix(sharedBody->localToMain(actorOffset), sharedBody->localToMain(responseOffset));

			const Mat3& rot = sharedBody->mainPhysical->getCFrame().getRotation();

			Mat3 globalResponse = rot * response * rot.transpose();

			systemToSolve.setSubMatrix(i * 3, j * 3, isPositive ? globalResponse : -globalResponse);
		}
	}

	return systemToSolve;
}


void ConstraintGroup::apply() const {
	size_t dimension = ballConstraints.size() * 3;
	LargeMatrix<double> systemToSolve = computeInteractionMatrix(*this);
	LargeVector<double> dragVector(dimension);
	LargeVector<double> velocityVector(dimension);
	LargeVector<double> accelerationVector(dimension);

	



	LargeMatrix<double> systemCopy(systemToSolve);
	LargeMatrix<double> systemCopyCopy(systemToSolve);

	size_t matrixIndex;

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
		bc.a->applyDragToPhysical(bc.a->getCFrame().localToRelative(bc.attachA), drag);
		bc.b->applyDragToPhysical(bc.b->getCFrame().localToRelative(bc.attachB), -drag);

		matrixIndex += 3;
	}

	// solve for velocity
	matrixIndex = 0;
	for (const BallConstraint& bc : ballConstraints) {
		Vec3 vb = bc.b->getMotionOfCenterOfMass().getVelocityOfPoint(bc.b->getCFrame().localToRelative(bc.attachB));
		Vec3 va = bc.a->getMotionOfCenterOfMass().getVelocityOfPoint(bc.a->getCFrame().localToRelative(bc.attachA));

		velocityVector.setSubVector(matrixIndex, vb - va);

		matrixIndex += 3;
	}
	destructiveSolve(systemToSolve, velocityVector);
	
	matrixIndex = 0;
	for (const BallConstraint& bc : ballConstraints) {
		Vec3 impulse;
		for (int i = 0; i < 3; i++) { impulse[i] = velocityVector[matrixIndex + i]; }
		bc.a->applyImpulseToPhysical(bc.a->getCFrame().localToRelative(bc.attachA), impulse);
		bc.b->applyImpulseToPhysical(bc.b->getCFrame().localToRelative(bc.attachB), -impulse);

		matrixIndex += 3;
	}

	// solve for acceleration
	matrixIndex = 0;
	for (const BallConstraint& bc : ballConstraints) {
		Vec3 ab = bc.b->getMotionOfCenterOfMass().getAccelerationOfPoint(bc.b->getCFrame().localToRelative(bc.attachB));
		Vec3 aa = bc.a->getMotionOfCenterOfMass().getAccelerationOfPoint(bc.a->getCFrame().localToRelative(bc.attachA));

		accelerationVector.setSubVector(matrixIndex, ab - aa);

		matrixIndex += 3;
	}

	destructiveSolve(systemCopy, accelerationVector);
	
	matrixIndex = 0;
	for (const BallConstraint& bc : ballConstraints) {
		Vec3 force;
		for (int i = 0; i < 3; i++) {force[i] = accelerationVector[matrixIndex + i];}
		bc.a->applyForceToPhysical(bc.a->getCFrame().localToRelative(bc.attachA), force);
		bc.b->applyForceToPhysical(bc.b->getCFrame().localToRelative(bc.attachB), -force);

		matrixIndex += 3;
	}
}
