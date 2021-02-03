#include "constraintGroup.h"

#include "../math/linalg/largeMatrix.h"
#include "../math/linalg/largeMatrixAlgorithms.h"
#include "../math/linalg/mat.h"
#include "../physical.h"

#include "../math/mathUtil.h"

#include "../misc/toString.h"
#include "../../util/log.h"

#include <fstream>
#include <cstddef>

#include <map>

int PhysicalConstraint::maxNumberOfParameters() const {
	return constraint->maxNumberOfParameters();
}
static PhysicalInfo getInfo(const Physical& phys) {
	GlobalCFrame cf = phys.getCFrame();
	Vec3 relativeCOM = phys.mainPhysical->totalCenterOfMass - (cf.getPosition() - phys.mainPhysical->getCFrame().getPosition());
	return PhysicalInfo{cf, phys.getMotion(), 1 / phys.mainPhysical->totalMass, phys.mainPhysical->momentResponse, relativeCOM};
}
ConstraintMatrixPack PhysicalConstraint::getMatrices(double* matrixBuf, double* errorBuf) const {
	return constraint->getMatrices(getInfo(*physA), getInfo(*physB), matrixBuf, errorBuf);
}

void ConstraintGroup::add(Physical* first, Physical* second, Constraint* constraint) {
	this->constraints.push_back(PhysicalConstraint(first, second, constraint));
}

void ConstraintGroup::apply() const {
	std::size_t maxNumberOfParameters = 0;
	ConstraintMatrixPack* constraintMatrices = new ConstraintMatrixPack[constraints.size()];

	for(std::size_t i = 0; i < constraints.size(); i++) {
		maxNumberOfParameters += constraints[i].constraint->maxNumberOfParameters();
	}

	double* matrixBuffer = new double[std::size_t(24) * maxNumberOfParameters];
	double* errorBuffer = new double[std::size_t(NUMBER_OF_ERROR_DERIVATIVES) * maxNumberOfParameters];

	std::size_t numberOfParams = 0;
	for(std::size_t i = 0; i < constraints.size(); i++) {
		constraintMatrices[i] = constraints[i].getMatrices(matrixBuffer + std::size_t(24) * numberOfParams, errorBuffer + std::size_t(NUMBER_OF_ERROR_DERIVATIVES) * numberOfParams);

		numberOfParams += constraintMatrices[i].getSize();
	}

	UnmanagedHorizontalFixedMatrix<double, NUMBER_OF_ERROR_DERIVATIVES> vectorToSolve(errorBuffer, maxNumberOfParameters);

	LargeMatrix<double> systemToSolve(numberOfParams, numberOfParams);
	{
		std::size_t curColIndex = 0;
		for(std::size_t blockCol = 0; blockCol < constraints.size(); blockCol++) {
			int colSize = constraintMatrices[blockCol].getSize();

			MotorizedPhysical* mPhysA = constraints[blockCol].physA->mainPhysical;
			MotorizedPhysical* mPhysB = constraints[blockCol].physB->mainPhysical;

			const UnmanagedHorizontalFixedMatrix<double, 6> motionToEq1 = constraintMatrices[blockCol].getMotionToEquationMatrixA();
			const UnmanagedHorizontalFixedMatrix<double, 6> motionToEq2 = constraintMatrices[blockCol].getMotionToEquationMatrixB();

			std::size_t curRowIndex = 0;
			for(std::size_t blockRow = 0; blockRow < constraints.size(); blockRow++) {
				int rowSize = constraintMatrices[blockRow].getSize();

				MotorizedPhysical* cPhysA = constraints[blockRow].physA->mainPhysical;
				MotorizedPhysical* cPhysB = constraints[blockRow].physB->mainPhysical;

				const UnmanagedVerticalFixedMatrix<double, 6> paramToMotion1 = constraintMatrices[blockRow].getParameterToMotionMatrixA();
				const UnmanagedVerticalFixedMatrix<double, 6> paramToMotion2 = constraintMatrices[blockRow].getParameterToMotionMatrixB();

				double resultBuf1[6 * 6]; UnmanagedLargeMatrix<double> resultMat1(resultBuf1, rowSize, colSize);
				for(double& d : resultMat1) d = 0.0;
				double resultBuf2[6 * 6]; UnmanagedLargeMatrix<double> resultMat2(resultBuf2, rowSize, colSize);
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
		std::size_t curParameterIndex = 0;
		for(std::size_t i = 0; i < constraints.size(); i++) {
			const UnmanagedVerticalFixedMatrix<double, 6> curP2MA = constraintMatrices[i].getParameterToMotionMatrixA();
			const UnmanagedVerticalFixedMatrix<double, 6> curP2MB = constraintMatrices[i].getParameterToMotionMatrixB();
			std::size_t curSize = curP2MA.cols;

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
		assert(curParameterIndex == numberOfParams);
	}
}
