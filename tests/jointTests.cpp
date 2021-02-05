#include "testsMain.h"

#include "compare.h"
#include "generators.h"
#include "estimateMotion.h"
#include "../physics/misc/toString.h"

#include "../physics/geometry/shape.h"
#include "../physics/geometry/shapeCreation.h"
#include "../physics/part.h"
#include "../physics/physical.h"
#include "../physics/layer.h"
#include "../physics/world.h"

#include "../physics/constraints/constraintGroup.h"
#include "../physics/constraints/ballConstraint.h"

#define ASSERT(cond) ASSERT_TOLERANT(cond, 0.05)

#define DELTA_T 0.0001

TEST_CASE(testBallConstraintMatrices) {
	Part firstPart(boxShape(1.0, 2.0, 3.0), GlobalCFrame(Position(0.0, 0.0, 0.0)), {0.3, 0.7, 0.9});
	Part secondPart(sphereShape(1.5), GlobalCFrame(Position(1.0, 0.0, 0.0)), {0.1, 0.6, 0.6});

	firstPart.ensureHasParent();
	secondPart.ensureHasParent();

	Vec3 attach1(0.0, 0.0, 0.0);
	Vec3 attach2(-1.0, 0.0, 0.0);

	BallConstraint bc(attach1, attach2);

	PhysicalConstraint pc(firstPart.parent, secondPart.parent, &bc);

	double matrixBuf[6 * 3 * 4 + 100];
	double errorBuf[NUMBER_OF_ERROR_DERIVATIVES * 3 + 100];


	ConstraintMatrixPack matrices = pc.getMatrices(matrixBuf, errorBuf);

	Vec3 param = generateVec3();
	LargeVector<double> paramBig(3);
	paramBig.setSubVector(param, 0);
	Vec6 motionA = matrices.getParameterToMotionMatrixA() * paramBig;

	firstPart.parent->applyImpulseToPhysical(attach1, param);
	ASSERT(firstPart.getMotion().getDerivAsVec6(0) == motionA);

	Vec6 motionB = -(matrices.getParameterToMotionMatrixB() * paramBig);

	secondPart.parent->applyImpulseToPhysical(attach2, -param);
	ASSERT(secondPart.getMotion().getDerivAsVec6(0) == motionB);
}

TEST_CASE(testBallConstraintSymmetric) {
	Part firstPart(generateShape(), generateGlobalCFrame(), {0.3, 0.7, 0.9});
	Part secondPart(generateShape(), generateGlobalCFrame(), {0.1, 0.6, 0.6});

	firstPart.ensureHasParent();
	secondPart.ensureHasParent();

	Vec3 attach1 = generateVec3();
	Vec3 attach2 = generateVec3();

	BallConstraint bcA(attach1, attach2);
	PhysicalConstraint pcA(firstPart.parent, secondPart.parent, &bcA);

	BallConstraint bcB(attach2, attach1);
	PhysicalConstraint pcB(secondPart.parent, firstPart.parent, &bcB);

	double matrixBufA[6 * 3 * 4 + 100];
	double errorBufA[NUMBER_OF_ERROR_DERIVATIVES * 3 + 100];
	double matrixBufB[6 * 3 * 4 + 100];
	double errorBufB[NUMBER_OF_ERROR_DERIVATIVES * 3 + 100];


	ConstraintMatrixPack matricesA = pcA.getMatrices(matrixBufA, errorBufA);
	ConstraintMatrixPack matricesB = pcB.getMatrices(matrixBufB, errorBufB);
	ASSERT(matricesA.getParameterToMotionMatrixA() == matricesB.getParameterToMotionMatrixB());
	ASSERT(matricesA.getParameterToMotionMatrixB() == matricesB.getParameterToMotionMatrixA());
	ASSERT(matricesA.getMotionToEquationMatrixA() == matricesB.getMotionToEquationMatrixB());
	ASSERT(matricesA.getMotionToEquationMatrixB() == matricesB.getMotionToEquationMatrixA());
	auto errB = matricesB.getErrorMatrix();
	inMemoryMatrixNegate(errB);
	ASSERT(matricesA.getErrorMatrix() == errB);
}


/*TEST_CASE(testBallConstraint) {
	Part part1(boxShape(2.0, 2.0, 2.0), GlobalCFrame(0.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	part1.ensureHasParent();
	Part part2(boxShape(2.0, 2.0, 2.0), GlobalCFrame(6.0, 0.0, 0.0), {1.0, 1.0, 1.0});
	part2.ensureHasParent();
	ConstraintGroup group;
	group.ballConstraints.push_back(BallConstraint{Vec3(3.0, 0.0, 0.0), part1.parent->mainPhysical, Vec3(-3.0, 0.0, 0.0), part2.parent->mainPhysical});

	part1.parent->mainPhysical->applyForceAtCenterOfMass(Vec3(2, 0, 0));

	group.apply();

	ASSERT(part1.getMotion().getAcceleration() == Vec3(0.125, 0.0, 0.0));
	ASSERT(part2.getMotion().getAcceleration() == Vec3(0.125, 0.0, 0.0));
}*/