#include "testsMain.h"
#include "compare.h"
#include "../physics/misc/toString.h"

#include "../physics/inertia.h"
#include "../physics/misc/shapeLibrary.h"

#include "randomValues.h"

#define REMAINS_CONSTANT(v) REMAINS_CONSTANT_TOLERANT(v, 0.0001)
#define ASSERT(v) ASSERT_TOLERANT(v, 0.0001)

#define DELTA_T 0.0001

TEST_CASE(movedInertialMatrixForBox) {
	Polyhedron originalShape = Library::createBox(1.0, 2.0, 3.0);
	Vec3 translation(3.1, -2.7, 7.9);
	Polyhedron translatedTriangle = originalShape.translated(translation);
	SymmetricMat3 triangleInertia = originalShape.getInertia(CFrame());
	SymmetricMat3 translatedTriangleInertia = translatedTriangle.getInertia(CFrame());

	ASSERT(getTranslatedInertia(triangleInertia, originalShape.getVolume(), translation, originalShape.getCenterOfMass()) == translatedTriangleInertia);
}

TEST_CASE(movedInertialMatrixForDifficuiltPart) {
	Polyhedron originalShape = Library::trianglePyramid;
	Vec3 translation(3.1, -2.7, 7.9);
	Polyhedron translatedTriangle = originalShape.translated(translation);
	SymmetricMat3 triangleInertia = originalShape.getInertia(CFrame());
	SymmetricMat3 translatedTriangleInertia = translatedTriangle.getInertia(CFrame());

	ASSERT(getTranslatedInertia(triangleInertia, originalShape.getVolume(), translation, originalShape.getCenterOfMass()) == translatedTriangleInertia);
}

TEST_CASE(inertiaTranslationDerivatives) {
	SymmetricMat3 inert = Library::trianglePyramid.getInertia(CFrame());
	double mass = Library::trianglePyramid.getVolume();

	TranslationalMotion motion(Vec3(0.3, 0.4, 0.5), Vec3(-0.8, 0.5, -0.3));

	Vec3 p1(0.8, 0.4, 0.2);
	Vec3 p2 = p1 + motion.getOffsetAfterDeltaT(DELTA_T);
	Vec3 p3 = p1 + motion.getOffsetAfterDeltaT(2 * DELTA_T);

	FullTaylor<SymmetricMat3> inertialTaylor = getTranslatedInertiaDerivativesAroundCenterOfMass(inert, mass, p1, motion);

	SymmetricMat3 i1 = getTranslatedInertiaAroundCenterOfMass(inert, mass, p1);
	SymmetricMat3 i2 = getTranslatedInertiaAroundCenterOfMass(inert, mass, p2);
	SymmetricMat3 i3 = getTranslatedInertiaAroundCenterOfMass(inert, mass, p3);

	ASSERT(i1 == inertialTaylor(0.0));
	ASSERT((i2 - i1)/DELTA_T == inertialTaylor.derivatives[0]);
	ASSERT((i3 + i1 - 2.0 * i2) / (DELTA_T * DELTA_T) == inertialTaylor.derivatives[1]);
}

TEST_CASE(inertiaRotationDerivatives) {
	SymmetricMat3 inert = Library::trianglePyramid.getInertia(CFrame());
	double mass = Library::trianglePyramid.getVolume();

	RotationalMotion motion(Vec3(0.3, 0.4, 0.5), Vec3(-0.8, 0.5, -0.3));

	Rotation r0 = Rotation::fromEulerAngles(0.8, 0.4, 0.2);
	Rotation r1 = Rotation::fromRotationVec(motion.getRotationAfterDeltaT(DELTA_T)) * r0;
	Rotation r2 = Rotation::fromRotationVec(motion.getRotationAfterDeltaT(2 * DELTA_T)) * r0;

	FullTaylor<SymmetricMat3> inertialTaylor = getRotatedInertiaTaylor(inert, r0, motion);

	SymmetricMat3 i1 = getRotatedInertia(inert, r0);
	SymmetricMat3 i2 = getRotatedInertia(inert, r1);
	SymmetricMat3 i3 = getRotatedInertia(inert, r2);

	ASSERT(i1 == inertialTaylor(0.0));
	ASSERT((i2 - i1) / DELTA_T == inertialTaylor.derivatives[0]);
	ASSERT((i3 + i1 - 2.0 * i2) / (DELTA_T * DELTA_T) == inertialTaylor.derivatives[1]);
}
