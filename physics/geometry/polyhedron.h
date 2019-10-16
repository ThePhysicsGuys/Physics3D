#pragma once

#include "../math/linalg/vec.h"
#include "../math/cframe.h"
#include "../math/transform.h"
#include "../datastructures/sharedArray.h"
#include "../datastructures/alignedPtr.h"
#include "../datastructures/iteratorFactory.h"
#include "genericCollidable.h"

#include <utility>

struct Triangle;
struct Polyhedron;
struct ShapeVecIter;
struct ShapeVecIterFactory;
struct ShapeTriangleIter;

#include "boundingBox.h"

size_t getOffset(size_t size);

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
	inline int operator[](int i) const {
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
	Triangle operator*() const {
		return Triangle{ *curTriangle, *(curTriangle + offset), *(curTriangle + 2 * offset) };
	}
	void operator++() {
		++curTriangle;
	}
	bool operator!=(const ShapeTriangleIter& other) const {
		return curTriangle != other.curTriangle;
	}
};

struct Polyhedron : public GenericCollidable {

	
private:
	SharedAlignedPointer<float> vertices;
	SharedAlignedPointer<int> triangles;
	const float* copyOfVerts() const;
	Polyhedron(const SharedAlignedPointer<float>& vertices, const SharedAlignedPointer<int>& triangles, int vertexCount, int triangleCount);
public:
	int vertexCount;
	int triangleCount;

	Polyhedron() : vertices(), triangles(), vertexCount(0), triangleCount(0) {};
	~Polyhedron();

	Polyhedron(const Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount);
	Polyhedron translated(Vec3f offset) const;
	Polyhedron rotated(RotMat3f rotation) const;
	Polyhedron localToGlobal(CFramef frame) const;
	Polyhedron globalToLocal(CFramef frame) const;
	Polyhedron scaled(float scaleX, float scaleY, float scaleZ) const;
	Polyhedron scaled(double scaleX, double scaleY, double scaleZ) const;

	bool isValid() const;
	bool containsPoint(Vec3f point) const;
	float getIntersectionDistance(Vec3f origin, Vec3f direction) const;
	double getVolume() const;

	//CFramef getInertialEigenVectors() const;
	BoundingBox getBounds() const;
	void computeNormals(Vec3f* buffer) const;
	Vec3 getCenterOfMass() const;
	SymmetricMat3 getInertia() const;
	SymmetricMat3 getInertia(Vec3 reference) const;
	SymmetricMat3 getInertia(Mat3 reference) const;
	SymmetricMat3 getInertia(CFrame reference) const;
	CircumscribingSphere getCircumscribingSphere() const;
	void getCircumscribedEllipsoid() const;
	double getMaxRadius() const;
	double getMaxRadius(Vec3f reference) const;
	double getMaxRadiusSq() const;
	double getMaxRadiusSq(Vec3f reference) const;
	Vec3f getNormalVecOfTriangle(Triangle triangle) const;

	int furthestIndexInDirection(const Vec3f& direction) const;
	virtual Vec3f furthestInDirection(const Vec3f& direction) const override;

	inline const float* getXVerts() const { return this->vertices.get(); }
	inline const float* getYVerts() const { return this->vertices.get() + getOffset(vertexCount); }
	inline const float* getZVerts() const { return this->vertices.get() + 2 * getOffset(vertexCount); }

	inline Vec3f operator[](int index) const {
		return Vec3f(this->getXVerts()[index], this->getYVerts()[index], this->getZVerts()[index]);
	}

	inline Triangle getTriangle(int index) const {
		size_t offset = getOffset(triangleCount);
		return Triangle{ triangles[index], triangles[index + offset], triangles[index + 2 * offset] };
	}

	IteratorFactory<ShapeVertexIter> iterVertices() const {
		return IteratorFactory<ShapeVertexIter>(ShapeVertexIter{ vertices, getOffset(vertexCount) }, ShapeVertexIter{ vertices + vertexCount, getOffset(vertexCount) });
	}
	IteratorFactory<ShapeTriangleIter> iterTriangles() const {
		return IteratorFactory<ShapeTriangleIter>(ShapeTriangleIter{ triangles, getOffset(triangleCount) }, ShapeTriangleIter{ triangles + triangleCount, getOffset(triangleCount) });
	}
};

