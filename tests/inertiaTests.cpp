#include "testsMain.h"
#include "compare.h"
#include "../physics/misc/toString.h"

#include "../physics/inertia.h"
#include "../physics/misc/shapeLibrary.h"

#include "simulation.h"

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

	Vec3 start(0.8, 0.4, 0.2);
	TranslationalMotion motion(Vec3(0.3, 0.4, 0.5), Vec3(-0.8, 0.5, -0.3));

	std::array<Vec3, 3> points = computeTranslationOverTime(start, motion, DELTA_T);
	std::array<SymmetricMat3, 3> inertias;
	for(int i = 0; i < 3; i++) {
		inertias[i] = getTranslatedInertiaAroundCenterOfMass(inert, mass, points[i]);
	}
	FullTaylor<SymmetricMat3> estimatedTaylor = estimateDerivatives(inertias, DELTA_T);
	FullTaylor<SymmetricMat3> inertialTaylor = getTranslatedInertiaDerivativesAroundCenterOfMass(inert, mass, start, motion);

	ASSERT(estimatedTaylor == inertialTaylor);
}

TEST_CASE(inertiaRotationDerivatives) {
	SymmetricMat3 inert = Library::trianglePyramid.getInertia(CFrame());

	Rotation start = Rotation::fromEulerAngles(0.8, 0.4, 0.2);
	RotationalMotion motion(Vec3(0.3, 0.4, 0.5), Vec3(-0.8, 0.5, -0.3));

	std::array<Rotation, 3> rotations = computeRotationOverTime(start, motion, DELTA_T);
	std::array<SymmetricMat3, 3> inertias;
	for(int i = 0; i < 3; i++) {
		inertias[i] = getRotatedInertia(inert, rotations[i]);
	}
	FullTaylor<SymmetricMat3> estimatedTaylor = estimateDerivatives(inertias, DELTA_T);
	FullTaylor<SymmetricMat3> inertialTaylor = getRotatedInertiaTaylor(inert, start, motion);

	ASSERT(estimatedTaylor == inertialTaylor);
}

TEST_CASE(inertiaTransformationDerivatives) {
	Polyhedron centeredTrianglePyramid = Library::trianglePyramid.translated(-Library::trianglePyramid.getCenterOfMass());

	SymmetricMat3 inert = centeredTrianglePyramid.getInertia(CFrame());
	double mass = centeredTrianglePyramid.getVolume();

	RotationalMotion rotation(Vec3(0.3, 0.4, 0.5), Vec3(-0.8, 0.5, -0.3));
	TranslationalMotion translation(Vec3(-0.23, 0.25, -0.7), Vec3(-0.2, -0.7, 0.333));
	Motion motion(translation, rotation);
	
	Vec3 startingTranslation(1.2, -0.7, 2.1);
	Rotation startingRotation = Rotation::fromEulerAngles(0.5, -0.6, 0.7);
	CFrame start(startingTranslation, startingRotation);

	std::array<CFrame, 3> cframes = computeCFrameOverTime(start, motion, DELTA_T);
	std::array<SymmetricMat3, 3> inertias;
	for(int i = 0; i < 3; i++) {
		inertias[i] = getTransformedInertiaAroundCenterOfMass(inert, mass, cframes[i]);
	}
	FullTaylor<SymmetricMat3> estimatedTaylor = estimateDerivatives(inertias, DELTA_T);
	FullTaylor<SymmetricMat3> inertialTaylor = getTransformedInertiaDerivativesAroundCenterOfMass(inert, mass, start, motion);

	ASSERT(estimatedTaylor == inertialTaylor);
}
TEST_CASE(inertiaTransformationDerivativesForOffsetCenterOfMass) {
	Vec3 com = Library::trianglePyramid.getCenterOfMass();

	SymmetricMat3 inert = Library::trianglePyramid.getInertia(CFrame());
	double mass = Library::trianglePyramid.getVolume();

	RotationalMotion rotation(Vec3(0.3, 0.4, 0.5), Vec3(-0.8, 0.5, -0.3));
	TranslationalMotion translation(Vec3(-0.23, 0.25, -0.7), Vec3(-0.2, -0.7, 0.333));
	Motion motion(translation, rotation);

	Vec3 startingTranslation(1.2, -0.7, 2.1);
	Rotation startingRotation = Rotation::fromEulerAngles(0.5, -0.6, 0.7);
	CFrame start(startingTranslation, startingRotation);

	std::array<CFrame, 3> cframes = computeCFrameOverTime(start, motion, DELTA_T);
	std::array<SymmetricMat3, 3> inertias;
	for(int i = 0; i < 3; i++) {
		inertias[i] = getTransformedInertiaAroundCenterOfMass(inert, mass, com, cframes[i]);
	}
	FullTaylor<SymmetricMat3> estimatedTaylor = estimateDerivatives(inertias, DELTA_T);
	FullTaylor<SymmetricMat3> inertialTaylor = getTransformedInertiaDerivativesAroundCenterOfMass(inert, mass, com, start, motion);

	ASSERT(estimatedTaylor == inertialTaylor);
}
