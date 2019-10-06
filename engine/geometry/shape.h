#pragma once


#include "boundingBox.h"
#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "polyhedron.h"

enum class ShapeType {
	SPHERE,
	POLYHEDRON,
};

class Shape {
	Polyhedron polyhedron;

public:
	Shape() : polyhedron() {};
	Shape(const Polyhedron& polyhedron) : polyhedron(polyhedron) {}
	Shape(Polyhedron&& poly) : polyhedron(std::move(poly)) {}

	inline bool containsPoint(Vec3f point) const { return polyhedron.containsPoint(point); };
	inline float getIntersectionDistance(Vec3f origin, Vec3f direction) const { return polyhedron.getIntersectionDistance(origin, direction); };
	inline double getVolume() const { return polyhedron.getVolume(); };
	
	inline Polyhedron scaled(float scaleX, float scaleY, float scaleZ) const { return polyhedron.scaled(scaleX, scaleY, scaleZ); };
	inline Polyhedron scaled(double scaleX, double scaleY, double scaleZ) const { return polyhedron.scaled(scaleX, scaleY, scaleZ); };
	
	inline BoundingBox getBounds() const { return polyhedron.getBounds(); };
	inline Vec3 getCenterOfMass() const { return polyhedron.getCenterOfMass(); };
	inline SymmetricMat3 getInertia() const { return polyhedron.getInertia(); };
	inline SymmetricMat3 getInertia(Vec3 reference) const { return polyhedron.getInertia(reference); };
	inline SymmetricMat3 getInertia(Mat3 reference) const { return polyhedron.getInertia(reference); };
	inline SymmetricMat3 getInertia(CFrame reference) const { return polyhedron.getInertia(reference); };
	inline CircumscribingSphere getCircumscribingSphere() const { return polyhedron.getCircumscribingSphere(); };
	inline double getMaxRadius() const { return polyhedron.getMaxRadius(); };
	inline double getMaxRadiusSq() const { return polyhedron.getMaxRadiusSq(); };

	bool intersectsTransformed(const Shape& other, const CFramef& relativeCFrame, Vec3f& intersection, Vec3f& exitVector) const {
		return polyhedron.intersectsTransformed(other.polyhedron, relativeCFrame, intersection, exitVector);
	};
	inline Vec3f furthestInDirection(const Vec3f& direction) const { return polyhedron.furthestInDirection(direction); };

	Polyhedron asPolyhedron() const { return polyhedron; }
};


