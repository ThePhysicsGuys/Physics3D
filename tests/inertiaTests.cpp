#include "testsMain.h"
#include "compare.h"
#include "../physics/misc/toString.h"

#include "../physics/inertia.h"
#include "../physics/misc/shapeLibrary.h"

#include "simulation.h"

#include "randomValues.h"

namespace P3D {
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

// test getting angular momentum for 2x2x2 box rotating at omega_x rad/s around the x axis
// moving at (0, vy, vz), at (0, 0, cz), relative to (0,0,0)
// answer computed by manually running through the angular momentum integral
// integrate(dz=1..2, dy=-1..1, dx=-1..1){offset % (angVel % offset + velocity)}
TEST_CASE(premadeAngularMomentum) {
	double omega_x = -2.1;
	double vy = 1.3;
	double vz = 2.9;
	double cz = 3.7;

	// 2x2x2 box
	double boxInertiaXX = 16.0 / 3.0;
	double boxVolume = 8.0;
	double selfBoxAngularMomentum = boxInertiaXX * omega_x;
	double velocityAngularMomentum = -boxVolume * cz * vy;
	double totalTrueAngularMomentum = selfBoxAngularMomentum + velocityAngularMomentum;

	SymmetricMat3 inertia{
		boxInertiaXX,
		0, boxInertiaXX,
		0, 0, boxInertiaXX
	};
	Vec3 angularVel(omega_x, 0, 0);
	Vec3 vel(0, vy, vz);
	Vec3 offset(0, 0, cz);

	Vec3 boxNotRotatingAngMom = getAngularMomentumFromOffsetOnlyVelocity(offset, vel, boxVolume);
	Vec3 boxNotRotatingAngMom2 = getAngularMomentumFromOffset(offset, vel, Vec3(0, 0, 0), inertia, boxVolume);
	ASSERT(boxNotRotatingAngMom == Vec3(velocityAngularMomentum, 0, 0));
	ASSERT(boxNotRotatingAngMom == boxNotRotatingAngMom2);

	Vec3 boxAlsoRotatingAngMom = getAngularMomentumFromOffset(offset, vel, angularVel, inertia, boxVolume);
	ASSERT(boxAlsoRotatingAngMom == Vec3(totalTrueAngularMomentum, 0, 0));
}

TEST_CASE(translatedAngularMomentum) {
	Polyhedron simpleBox = Library::createCube(1.0f);

	SymmetricMat3 inertia = simpleBox.getInertiaAroundCenterOfMass();
	double mass = simpleBox.getVolume();

	Vec3 offset(1.3, 0.7, -2.1);
	Vec3 angularVel(3.2, 1.3, -2.1);

	SymmetricMat3 offsetInertia = getTranslatedInertiaAroundCenterOfMass(inertia, mass, offset);

	Vec3 angularMomentumTarget = offsetInertia * angularVel;
	Vec3 rotationAngularMomentum = inertia * angularVel;
	Vec3 velocity = angularVel % offset;
	Vec3 angularMomentumFromVelocity = offset % velocity * mass;
	Vec3 angularMomentumTest = rotationAngularMomentum + angularMomentumFromVelocity;

	Vec3 computedAngularMomentumFromVelocity = getAngularMomentumFromOffsetOnlyVelocity(offset, velocity, mass);
	Vec3 computedAngularMomentumFromVelocityAndAngular = getAngularMomentumFromOffset(offset, velocity, angularVel, inertia, mass);

	ASSERT(angularMomentumTarget == angularMomentumTest);
	ASSERT(angularMomentumFromVelocity == computedAngularMomentumFromVelocity);
	ASSERT(angularMomentumTarget == computedAngularMomentumFromVelocityAndAngular);
}
};