#pragma once

#include "../math/linalg/vec.h"
#include "../math/cframe.h"
#include "../math/transform.h"
#include "boundingBox.h"
#include "../datastructures/sharedArray.h"
#include "../datastructures/alignedPtr.h"
#include "../datastructures/iteratorFactory.h"
#include "genericCollidable.h"
#include "scalableInertialMatrix.h"

#include <utility>
#include <iosfwd>

struct Triangle;
struct Polyhedron;
struct ShapeVecIter;
struct ShapeVecIterFactory;
struct ShapeTriangleIter;

struct Triangle {
	union {
		struct { 
			int firstIndex, secondIndex, thirdIndex; 
		};
		int indexes[3];
	};

	bool sharesEdgeWith(Triangle other) const;
	Triangle rightShift() const;
	Triangle leftShift() const;
	Triangle operator~() const;
	bool operator==(const Triangle& other) const;
	inline int& operator[](int i) {
		return indexes[i];
	};
	inline const int& operator[](int i) const {
		return indexes[i]; 
	};
};

struct ShapeVertexIter {
	float* curVertex;
	size_t offset;
	Vec3f operator*() const {
		return Vec3f{ *curVertex, *(curVertex + offset), *(curVertex + 2 * offset) };
	}
	void operator++() {
		++curVertex;
	}
	bool operator!=(const ShapeVertexIter& other) const {
		return curVertex != other.curVertex;
	}
};

struct ShapeTriangleIter {
	int* curTriangle;
	size_t offset;
	inline Triangle operator*() const {
		return Triangle{ *curTriangle, *(curTriangle + offset), *(curTriangle + 2 * offset) };
	}
	inline void operator++() {
		++curTriangle;
	}
	inline bool operator!=(const ShapeTriangleIter& other) const {
		return curTriangle != other.curTriangle;
	}
	inline bool operator==(const ShapeTriangleIter& other) const {
		return curTriangle == other.curTriangle;
	}
};

class NormalizedPolyhedron;
class Shape;

struct Polyhedron : public GenericCollidable {
private:
	UniqueAlignedPointer<float> vertices;
	UniqueAlignedPointer<int> triangles;
	Polyhedron(UniqueAlignedPointer<float>&& vertices, UniqueAlignedPointer<int>&& triangles, int vertexCount, int triangleCount);
public:
	int vertexCount;
	int triangleCount;

	Polyhedron() : vertices(), triangles(), vertexCount(0), triangleCount(0) {};
	~Polyhedron();

	Polyhedron(const Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount);
	Polyhedron(Polyhedron&& poly) noexcept : vertices(std::move(poly.vertices)), triangles(std::move(poly.triangles)), vertexCount(poly.vertexCount), triangleCount(poly.triangleCount) {}
	Polyhedron(const Polyhedron& poly);
	Polyhedron& operator=(Polyhedron&& poly) noexcept;
	Polyhedron& operator=(const Polyhedron& poly);

	Polyhedron translated(Vec3f offset) const;
	Polyhedron rotated(RotMat3f rotation) const;
	Polyhedron localToGlobal(CFramef frame) const;
	Polyhedron globalToLocal(CFramef frame) const;
	Polyhedron scaled(float scaleX, float scaleY, float scaleZ) const;
	Polyhedron scaled(double scaleX, double scaleY, double scaleZ) const;
	Polyhedron scaled(DiagonalMat3 scale) const { return scaled(scale[0], scale[1], scale[2]); }
	Polyhedron translatedAndScaled(Vec3f translation, DiagonalMat3f scale) const;

	bool isValid() const;
	bool containsPoint(Vec3f point) const;
	float getIntersectionDistance(Vec3f origin, Vec3f direction) const;
	double getVolume() const;

	BoundingBox getBounds() const;
	BoundingBox getBounds(const Mat3f& referenceFrame) const;
	void computeNormals(Vec3f* buffer) const;
	Vec3 getCenterOfMass() const;
	SymmetricMat3 getInertiaAroundCenterOfMass() const;
	SymmetricMat3 getInertia(const CFrame& reference) const;
	ScalableInertialMatrix getScalableInertia(const CFrame& reference) const;
	ScalableInertialMatrix getScalableInertiaAroundCenterOfMass() const;
	CircumscribingSphere getCircumscribingSphere() const;
	void getCircumscribedEllipsoid() const;
	double getMaxRadius() const;
	double getMaxRadius(Vec3f reference) const;
	double getMaxRadiusSq() const;
	double getMaxRadiusSq(Vec3f reference) const;
	double getScaledMaxRadius(DiagonalMat3 scale) const;
	double getScaledMaxRadiusSq(DiagonalMat3 scale) const;
	Vec3f getNormalVecOfTriangle(Triangle triangle) const;

	int furthestIndexInDirection(const Vec3f& direction) const;
	virtual Vec3f furthestInDirection(const Vec3f& direction) const override;

	Vec3f operator[](int index) const;

	NormalizedPolyhedron normalized() const;
	operator Shape() const;

	Triangle getTriangle(int index) const;

	IteratorFactory<ShapeVertexIter> iterVertices() const;
	IteratorFactory<ShapeTriangleIter> iterTriangles() const;

	void getTriangles(Triangle* triangleBuf) const;
	void getVertices(Vec3f* vertexBuf) const;
};
