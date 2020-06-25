#include "constraintGroup.h"

#include "math/linalg/largeMatrix.h"
#include "physical.h"
#include "math/linalg/mat.h"

#include "math/mathUtil.h"
#include <fstream>


void ConstraintGroup::add(Physical* first, Physical* second, BallConstraint* constraint) {
	this->constraints.push_back(PhysicalConstraint(first, second, constraint));
}
void BallConstraint::doNothing() {}

LargeMatrix<double> computeInteractionMatrix(const ConstraintGroup& group) {
	const std::vector<PhysicalConstraint>& constraints = group.constraints;

	size_t dimension = constraints.size() * 3;
	LargeMatrix<double> systemToSolve(dimension, dimension);
	for (size_t i = 0; i < dimension; i++) {
		for (size_t j = 0; j < dimension; j++) {
			systemToSolve.get(i, j) = 0.0;
		}
	}

	size_t matrixIndex = 0;
	for (const PhysicalConstraint& pc : constraints) {
		const BallConstraint& bc = *pc.constraint;
		/*Local to A*/ SymmetricMat3 responseA = pc.physA->mainPhysical->getResponseMatrix(pc.physA->localToMain(bc.attachA));
		/*Local to B*/ SymmetricMat3 responseB = pc.physB->mainPhysical->getResponseMatrix(pc.physB->localToMain(bc.attachB));
		GlobalCFrame cfA = pc.physA->mainPhysical->getCFrame();
		GlobalCFrame cfB = pc.physB->mainPhysical->getCFrame();
		/*Global?*/ SymmetricMat3 selfResponse = cfA.rotation.localToGlobal(responseA) + cfB.rotation.localToGlobal(responseB);

		systemToSolve.setSubMatrix(matrixIndex, matrixIndex, Mat3(selfResponse));

		matrixIndex += 3;
	}

	for (size_t i = 0; i < constraints.size(); i++) {
		const PhysicalConstraint& y = constraints[i];

		for (size_t j = 0; j < constraints.size(); j++) {
			if (i == j) continue;
			const PhysicalConstraint& x = constraints[j];

			// find effect of y constraint on velocities of x;

			bool isPositive;
			Physical* sharedBody;
			Vec3 actorOffset;
			Vec3 responseOffset;

				 if (x.physA == y.physA) { 
					 isPositive = true;  sharedBody = x.physA; actorOffset = x.constraint->attachA; responseOffset = y.constraint->attachA; }
			else if (x.physA == y.physB) { 
					 isPositive = false; sharedBody = x.physA; actorOffset = x.constraint->attachA; responseOffset = y.constraint->attachB; }
			else if (x.physB == y.physA) { 
					 isPositive = false; sharedBody = x.physB; actorOffset = x.constraint->attachB; responseOffset = y.constraint->attachA; }
			else if (x.physA == y.physB) { 
					 isPositive = true;  sharedBody = x.physB; actorOffset = x.constraint->attachB; responseOffset = y.constraint->attachB; }
			else {continue;}
			
			Mat3 response = sharedBody->mainPhysical->getResponseMatrix(sharedBody->localToMain(actorOffset), sharedBody->localToMain(responseOffset));

			Mat3 rot = sharedBody->mainPhysical->getCFrame().getRotation().asRotationMatrix();

			Mat3 globalResponse = rot * response * rot.transpose();

			systemToSolve.setSubMatrix(i * 3, j * 3, isPositive ? globalResponse : -globalResponse);
		}
	}

	return systemToSolve;
}


void ConstraintGroup::apply() const {
	size_t dimension = constraints.size() * 3;
	LargeMatrix<double> systemToSolve = computeInteractionMatrix(*this);
	LargeVector<double> dragVector(dimension);
	LargeVector<double> velocityVector(dimension);
	LargeVector<double> accelerationVector(dimension);

	



	LargeMatrix<double> systemCopy(systemToSolve);
	LargeMatrix<double> systemCopyCopy(systemToSolve);

	size_t matrixIndex;

	// solve for position
	matrixIndex = 0;
	for (const PhysicalConstraint& bc : constraints) {
		Position posA = bc.physA->getCFrame().localToGlobal(bc.constraint->attachA);
		Position posB = bc.physB->getCFrame().localToGlobal(bc.constraint->attachB);
		dragVector.setSubVector(matrixIndex, Vec3(posB - posA));

		matrixIndex += 3;
	}
	destructiveSolve(systemCopyCopy, dragVector);

	matrixIndex = 0;
	for (const PhysicalConstraint& bc : constraints) {
		Vec3 drag;
		for (int i = 0; i < 3; i++) { drag[i] = dragVector[matrixIndex + i]; }
		bc.physA->applyDragToPhysical(bc.physA->getCFrame().localToRelative(bc.constraint->attachA), drag);
		bc.physB->applyDragToPhysical(bc.physB->getCFrame().localToRelative(bc.constraint->attachB), -drag);

		matrixIndex += 3;
	}

	// solve for velocity
	matrixIndex = 0;
	for (const PhysicalConstraint& bc : constraints) {
		Vec3 vb = bc.physB->getMotionOfCenterOfMass().getVelocityOfPoint(bc.physB->getCFrame().localToRelative(bc.constraint->attachB));
		Vec3 va = bc.physA->getMotionOfCenterOfMass().getVelocityOfPoint(bc.physA->getCFrame().localToRelative(bc.constraint->attachA));

		velocityVector.setSubVector(matrixIndex, vb - va);

		matrixIndex += 3;
	}
	destructiveSolve(systemToSolve, velocityVector);
	
	matrixIndex = 0;
	for (const PhysicalConstraint& bc : constraints) {
		Vec3 impulse;
		for (int i = 0; i < 3; i++) { impulse[i] = velocityVector[matrixIndex + i]; }
		bc.physA->applyImpulseToPhysical(bc.physA->getCFrame().localToRelative(bc.constraint->attachA), impulse);
		bc.physB->applyImpulseToPhysical(bc.physB->getCFrame().localToRelative(bc.constraint->attachB), -impulse);

		matrixIndex += 3;
	}

	// solve for acceleration
	matrixIndex = 0;
	for (const PhysicalConstraint& bc : constraints) {
		Vec3 ab = bc.physB->getMotionOfCenterOfMass().getAccelerationOfPoint(bc.physB->getCFrame().localToRelative(bc.constraint->attachB));
		Vec3 aa = bc.physA->getMotionOfCenterOfMass().getAccelerationOfPoint(bc.physA->getCFrame().localToRelative(bc.constraint->attachA));

		accelerationVector.setSubVector(matrixIndex, ab - aa);

		matrixIndex += 3;
	}

	destructiveSolve(systemCopy, accelerationVector);
	
	matrixIndex = 0;
	for (const PhysicalConstraint& bc : constraints) {
		Vec3 force;
		for (int i = 0; i < 3; i++) {force[i] = accelerationVector[matrixIndex + i];}
		bc.physA->applyForceToPhysical(bc.physA->getCFrame().localToRelative(bc.constraint->attachA), force);
		bc.physB->applyForceToPhysical(bc.physB->getCFrame().localToRelative(bc.constraint->attachB), -force);

		matrixIndex += 3;
	}
}
