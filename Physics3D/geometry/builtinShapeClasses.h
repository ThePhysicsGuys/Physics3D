#pragma once

#include "polyhedron.h"
#include "shapeClass.h"

namespace P3D {
#define CUBE_CLASS_ID 0
#define SPHERE_CLASS_ID 1
#define CYLINDER_CLASS_ID 2
#define WEDGE_CLASS_ID 3
#define CORNER_CLASS_ID 4
#define CONVEX_POLYHEDRON_CLASS_ID 10


class CubeClass : public ShapeClass {
	CubeClass();

public:
	virtual bool containsPoint(Vec3 point) const;
	virtual double getIntersectionDistance(Vec3 origin, Vec3 direction) const;
	virtual BoundingBox getBounds(const Rotation& rotation, const DiagonalMat3& scale) const;
	virtual double getScaledMaxRadiusSq(DiagonalMat3 scale) const;
	virtual Vec3f furthestInDirection(const Vec3f& direction) const;
	virtual Polyhedron asPolyhedron() const;

	static const CubeClass instance;
};

class SphereClass : public ShapeClass {
	SphereClass();

public:
	virtual bool containsPoint(Vec3 point) const;
	virtual double getIntersectionDistance(Vec3 origin, Vec3 direction) const;
	virtual BoundingBox getBounds(const Rotation& rotation, const DiagonalMat3& scale) const;
	virtual double getScaledMaxRadiusSq(DiagonalMat3 scale) const;
	virtual double getScaledMaxRadius(DiagonalMat3 scale) const;
	virtual Vec3f furthestInDirection(const Vec3f& direction) const;
	virtual Polyhedron asPolyhedron() const;
	void setScaleX(double newX, DiagonalMat3& scale) const override;
	void setScaleY(double newY, DiagonalMat3& scale) const override;
	void setScaleZ(double newZ, DiagonalMat3& scale) const override;

	static const SphereClass instance;
};

class CylinderClass : public ShapeClass {
	CylinderClass();

public:
	virtual bool containsPoint(Vec3 point) const;
	virtual double getIntersectionDistance(Vec3 origin, Vec3 direction) const;
	virtual BoundingBox getBounds(const Rotation& rotation, const DiagonalMat3& scale) const;
	virtual double getScaledMaxRadiusSq(DiagonalMat3 scale) const;
	virtual Vec3f furthestInDirection(const Vec3f& direction) const;
	virtual Polyhedron asPolyhedron() const;
	void setScaleX(double newX, DiagonalMat3& scale) const override;
	void setScaleY(double newY, DiagonalMat3& scale) const override;

	static const CylinderClass instance;
};

class WedgeClass : public ShapeClass {
	WedgeClass();

public:
	virtual bool containsPoint(Vec3 point) const override;
	virtual double getIntersectionDistance(Vec3 origin, Vec3 direction) const override;
	virtual BoundingBox getBounds(const Rotation& rotation, const DiagonalMat3& scale) const override;
	virtual double getScaledMaxRadiusSq(DiagonalMat3 scale) const override;
	virtual Vec3f furthestInDirection(const Vec3f& direction) const override;
	virtual Polyhedron asPolyhedron() const override;

	static const WedgeClass instance;
};

class CornerClass : public ShapeClass {
	CornerClass();

public:
	virtual bool containsPoint(Vec3 point) const override;
	virtual double getIntersectionDistance(Vec3 origin, Vec3 direction) const override;
	virtual BoundingBox getBounds(const Rotation& rotation, const DiagonalMat3& scale) const override;
	virtual double getScaledMaxRadiusSq(DiagonalMat3 scale) const override;
	virtual Vec3f furthestInDirection(const Vec3f& direction) const override;
	virtual Polyhedron asPolyhedron() const override;

	static const CornerClass instance;

};

class PolyhedronShapeClass : public ShapeClass {
protected:
	Polyhedron poly;
public:
	PolyhedronShapeClass(Polyhedron&& poly) noexcept;

	virtual bool containsPoint(Vec3 point) const override;
	virtual double getIntersectionDistance(Vec3 origin, Vec3 direction) const override;
	virtual BoundingBox getBounds(const Rotation& rotation, const DiagonalMat3& scale) const override;
	virtual double getScaledMaxRadius(DiagonalMat3 scale) const override;
	virtual double getScaledMaxRadiusSq(DiagonalMat3 scale) const override;
	virtual Vec3f furthestInDirection(const Vec3f& direction) const override;
	virtual Polyhedron asPolyhedron() const override;
};

class PolyhedronShapeClassAVX : public PolyhedronShapeClass {
public:
	using PolyhedronShapeClass::PolyhedronShapeClass;

	virtual BoundingBox getBounds(const Rotation& rotation, const DiagonalMat3& scale) const override;
	virtual Vec3f furthestInDirection(const Vec3f& direction) const override;
};

class PolyhedronShapeClassSSE : public PolyhedronShapeClass {
public:
	using PolyhedronShapeClass::PolyhedronShapeClass;

	virtual BoundingBox getBounds(const Rotation& rotation, const DiagonalMat3& scale) const override;
	virtual Vec3f furthestInDirection(const Vec3f& direction) const override;
};

class PolyhedronShapeClassSSE4 : public PolyhedronShapeClass {
public:
	using PolyhedronShapeClass::PolyhedronShapeClass;

	virtual BoundingBox getBounds(const Rotation& rotation, const DiagonalMat3& scale) const override;
	virtual Vec3f furthestInDirection(const Vec3f& direction) const override;
};

class PolyhedronShapeClassFallback : public PolyhedronShapeClass {
public:
	using PolyhedronShapeClass::PolyhedronShapeClass;

	virtual BoundingBox getBounds(const Rotation& rotation, const DiagonalMat3& scale) const override;
	virtual Vec3f furthestInDirection(const Vec3f& direction) const override;
};
};