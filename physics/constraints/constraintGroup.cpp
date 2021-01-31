#include "constraintGroup.h"

#include "../math/linalg/largeMatrix.h"
#include "../math/linalg/largeMatrixAlgorithms.h"
#include "../math/linalg/mat.h"
#include "../physical.h"

#include "../math/mathUtil.h"

#include "../misc/toString.h"
#include "../../util/log.h"

#include <fstream>

#include <map>


void PhysicalConstraint::getMatrices(UnmanagedVerticalFixedMatrix<double, 6>& parameterToMotionMatrixA, UnmanagedVerticalFixedMatrix<double, 6>& parameterToMotionMatrixB, UnmanagedHorizontalFixedMatrix<double, 6>& motionToEquationMatrixA, UnmanagedHorizontalFixedMatrix<double, 6>& motionToEquationMatrixB, UnmanagedHorizontalFixedMatrix<double, NUMBER_OF_ERROR_DERIVATIVES>& errorValue) const {
	constraint->getMatrices(*physA, *physB, parameterToMotionMatrixA, parameterToMotionMatrixB, motionToEquationMatrixA, motionToEquationMatrixB, errorValue);	
}


void ConstraintGroup::add(Physical* first, Physical* second, Constraint* constraint) {
	this->constraints.push_back(PhysicalConstraint(first, second, constraint));
}


void ConstraintGroup::apply() const {
	size_t numberOfParameters = 0;
	UnmanagedVerticalFixedMatrix<double, 6>* parameterToMotion = new UnmanagedVerticalFixedMatrix<double, 6>[constraints.size() * 2];
	UnmanagedHorizontalFixedMatrix<double, 6>* motionToEquation = new UnmanagedHorizontalFixedMatrix<double, 6>[constraints.size() * 2];
	for(size_t i = 0; i < constraints.size(); i++) {
		int numParameters = constraints[i].constraint->numberOfParameters();
		parameterToMotion[i * 2].cols = numParameters;
		parameterToMotion[i * 2 + 1].cols = numParameters;
		motionToEquation[i * 2].rows = numParameters;
		motionToEquation[i * 2 + 1].rows = numParameters;
		numberOfParameters += numParameters;
	}

	double* matrixBuffer = new double[6 * numberOfParameters * 4];
	HorizontalFixedMatrix<double, NUMBER_OF_ERROR_DERIVATIVES> vectorToSolve(numberOfParameters);

	{
		size_t curParameterIndex = 0;
		for(size_t i = 0; i < constraints.size(); i++) {
			size_t curSize = parameterToMotion[i].cols;
			parameterToMotion[i * 2].data = matrixBuffer + 6 * 4 * curParameterIndex;
			parameterToMotion[i * 2 + 1].data = matrixBuffer + 6 * (4 * curParameterIndex + curSize);
			motionToEquation[i * 2].data = matrixBuffer + 6 * (4 * curParameterIndex + 2 * curSize);
			motionToEquation[i * 2 + 1].data = matrixBuffer + 6 * (4 * curParameterIndex + 3 * curSize);

			// writing to errorVec which writes to vectorToSolve
			UnmanagedHorizontalFixedMatrix<double, NUMBER_OF_ERROR_DERIVATIVES> errorVec = vectorToSolve.subRows(curParameterIndex, curSize);
			constraints[i].getMatrices(parameterToMotion[i * 2], parameterToMotion[i * 2 + 1], motionToEquation[i * 2], motionToEquation[i * 2 + 1], errorVec);

			curParameterIndex += curSize;
		}
		assert(curParameterIndex == numberOfParameters);
	}

	LargeMatrix<double> systemToSolve(numberOfParameters, numberOfParameters);

	{
		size_t curColIndex = 0;
		for(size_t blockCol = 0; blockCol < constraints.size(); blockCol++) {
			int colSize = parameterToMotion[blockCol * 2].cols;

			MotorizedPhysical* mPhysA = constraints[blockCol].physA->mainPhysical;
			MotorizedPhysical* mPhysB = constraints[blockCol].physB->mainPhysical;

			const UnmanagedHorizontalFixedMatrix<double, 6> & motionToEq1 = motionToEquation[blockCol * 2];
			const UnmanagedHorizontalFixedMatrix<double, 6> & motionToEq2 = motionToEquation[blockCol * 2 + 1];

			size_t curRowIndex = 0;
			for(size_t blockRow = 0; blockRow < constraints.size(); blockRow++) {
				int rowSize = motionToEquation[blockRow * 2].rows;

				MotorizedPhysical* cPhysA = constraints[blockRow].physA->mainPhysical;
				MotorizedPhysical* cPhysB = constraints[blockRow].physB->mainPhysical;

				const UnmanagedVerticalFixedMatrix<double, 6> & paramToMotion1 = parameterToMotion[blockRow * 2];
				const UnmanagedVerticalFixedMatrix<double, 6> & paramToMotion2 = parameterToMotion[blockRow * 2 + 1];

				double resultBuf1[6 * 6];
				UnmanagedLargeMatrix<double> resultMat1(resultBuf1, rowSize, colSize);
				for(double& d : resultMat1) d = 0.0;
				double resultBuf2[6 * 6];
				UnmanagedLargeMatrix<double> resultMat2(resultBuf2, rowSize, colSize);
				for(double& d : resultMat2) d = 0.0;
				if(mPhysA == cPhysA) {
					inMemoryMatrixMultiply(motionToEq1, paramToMotion1, resultMat1);
				} else if(mPhysA == cPhysB) {
					inMemoryMatrixMultiply(motionToEq1, paramToMotion2, resultMat1);
					inMemoryMatrixNegate(resultMat1);
				}
				if(mPhysB == cPhysA) {
					inMemoryMatrixMultiply(motionToEq2, paramToMotion1, resultMat2);
					inMemoryMatrixNegate(resultMat2);
				} else if(mPhysB == cPhysB) {
					inMemoryMatrixMultiply(motionToEq2, paramToMotion2, resultMat2);
				}

				resultMat1 += resultMat2;

				systemToSolve.setSubMatrix(curRowIndex, curColIndex, resultMat1);

				curRowIndex += rowSize;
			}
			curColIndex += colSize;
		}
	}

	Log::debug("System matrix: %s\nvector: %s", str(systemToSolve).c_str(), str(vectorToSolve).c_str());

	destructiveSolve(systemToSolve, vectorToSolve);

	{
		size_t curParameterIndex = 0;
		for(size_t i = 0; i < constraints.size(); i++) {
			const UnmanagedVerticalFixedMatrix<double, 6>& curP2MA = parameterToMotion[i * 2];
			const UnmanagedVerticalFixedMatrix<double, 6>& curP2MB = parameterToMotion[i * 2 + 1];
			size_t curSize = curP2MA.cols;

			UnmanagedHorizontalFixedMatrix<double, NUMBER_OF_ERROR_DERIVATIVES> parameterVec = vectorToSolve.subRows(curParameterIndex, curSize);
			Matrix<double, 6, NUMBER_OF_ERROR_DERIVATIVES> effectOnA = curP2MA * parameterVec;
			Matrix<double, 6, NUMBER_OF_ERROR_DERIVATIVES> effectOnB = -(curP2MB * parameterVec);

			// TODO add moving correction
			Vector<double, 6> offsetAngularEffectOnA = effectOnA.getCol(0);
			Vector<double, 6> offsetAngularEffectOnB = effectOnB.getCol(0);

			GlobalCFrame& mainPACF = constraints[i].physA->mainPhysical->rigidBody.mainPart->cframe;
			GlobalCFrame& mainPBCF = constraints[i].physB->mainPhysical->rigidBody.mainPart->cframe;
			mainPACF.position += offsetAngularEffectOnA.getSubVector<3>(0);
			mainPACF.rotation = Rotation::fromRotationVec(offsetAngularEffectOnA.getSubVector<3>(3)) * mainPACF.rotation;
			mainPBCF.position += offsetAngularEffectOnB.getSubVector<3>(0);
			mainPBCF.rotation = Rotation::fromRotationVec(offsetAngularEffectOnB.getSubVector<3>(3)) * mainPBCF.rotation;

			Vector<double, 6> velAngularEffectOnA = effectOnA.getCol(1);
			Vector<double, 6> velAngularEffectOnB = effectOnB.getCol(1);
			constraints[i].physA->mainPhysical->motionOfCenterOfMass.translation.translation[0] += velAngularEffectOnA.getSubVector<3>(0);
			constraints[i].physA->mainPhysical->motionOfCenterOfMass.rotation.rotation[0] += velAngularEffectOnA.getSubVector<3>(3);
			constraints[i].physB->mainPhysical->motionOfCenterOfMass.translation.translation[0] += velAngularEffectOnB.getSubVector<3>(0);
			constraints[i].physB->mainPhysical->motionOfCenterOfMass.rotation.rotation[0] += velAngularEffectOnB.getSubVector<3>(3);


			/*Vector<double, 6> accelAngularEffectOnA = effectOnA.getCol(2);
			Vector<double, 6> accelAngularEffectOnB = effectOnB.getCol(2);
			constraints[i].physA->mainPhysical->totalForce += constraints[i].physA->mainPhysical->totalMass * velAngularEffectOnA.getSubVector<3>(0);
			constraints[i].physA->mainPhysical->totalMoment += ~constraints[i].physA->mainPhysical->momentResponse * velAngularEffectOnA.getSubVector<3>(3);
			constraints[i].physB->mainPhysical->totalForce += constraints[i].physB->mainPhysical->totalMass * velAngularEffectOnB.getSubVector<3>(0);
			constraints[i].physB->mainPhysical->totalMoment += ~constraints[i].physB->mainPhysical->momentResponse * velAngularEffectOnB.getSubVector<3>(3);*/


			curParameterIndex += curSize;
		}
		assert(curParameterIndex == numberOfParameters);
	}
}
