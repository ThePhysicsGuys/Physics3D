#pragma once

#include "../math/vec.h"
#include "../math/cframe.h"
#include "../datastructures/parallelVector.h"
#include "../datastructures/sharedArray.h"

struct Triangle;
struct Shape;
struct ShapeVecIter;
struct ShapeVecIterFactory;

#include "boundingBox.h"
#include "../math/position.h"

struct Sphere {
	Vec3 origin = Vec3();
	double radius = 0;
};

struct GlobalSphere {
	Position origin = Position();
	double radius = 0;
};

struct Triangle {
	union {
		struct { 
			unsigned int firstIndex, secondIndex, thirdIndex; 
		};
		unsigned int indexes[3];
	};

	bool sharesEdgeWith(Triangle other) const;
	Triangle rightShift() const;
	Triangle leftShift() const;
	Triangle operator~() const;
	bool operator==(const Triangle& other) const;
	inline unsigned int operator[](int i) const {
		return indexes[i]; 
	};
};


struct VertexIterFactory {
	const Vec3f* verts;
	int size;
	inline const Vec3f* begin() const {
		return verts;
	};
	inline const Vec3f* end() const {
		return verts + size;
	};
};


struct Shape {
	struct TriangleIter {
		const Triangle* first;
		int size;
		inline const Triangle* begin() const { 
			return first;
		};
		inline const Triangle* end() const { 
			return first + size; 
		};
	};

	
private:
	SharedArrayPtr<const ParallelVec3> vertices;
	Shape(const ParallelVec3* vertices, const SharedArrayPtr<const Triangle>& triangles, int vertexCount, int triangleCount);
	Shape(const SharedArrayPtr<const ParallelVec3>& vertices, const SharedArrayPtr<const Triangle>& triangles, int vertexCount, int triangleCount) : vertices(vertices), triangles(triangles), vertexCount(vertexCount), triangleCount(triangleCount) {};
public:
	SharedArrayPtr<const Triangle> triangles;
	int vertexCount;
	int triangleCount;

	Shape() : vertices(nullptr), triangles(SharedArrayPtr<const Triangle>::staticSharedArrayPtr(nullptr)), vertexCount(0), triangleCount(0) {};
	Shape(const Vec3f* vertices, const SharedArrayPtr<const Triangle>& triangles, int vertexCount, int triangleCount);
	Shape translated(Vec3f offset) const;
	Shape rotated(RotMat3f rotation) const;
	Shape localToGlobal(CFramef frame) const;
	Shape globalToLocal(CFramef frame) const;
	Shape scaled(float scaleX, float scaleY, float scaleZ) const;

	bool isValid() const;
	bool containsPoint(Vec3f point) const;
	float getIntersectionDistance(Vec3f origin, Vec3f direction) const;
	double getVolume() const;

	CFramef getInertialEigenVectors() const;
	BoundingBox getBounds() const;
	void computeNormals(Vec3f* buffer) const;
	Vec3 getCenterOfMass() const;
	SymmetricMat3 getInertia() const;
	SymmetricMat3 getInertia(Vec3 reference) const;
	SymmetricMat3 getInertia(Mat3 reference) const;
	SymmetricMat3 getInertia(CFrame reference) const;
	Sphere getCircumscribingSphere() const;
	void getCircumscribedEllipsoid() const;
	double getMaxRadius() const;
	double getMaxRadius(Vec3f reference) const;
	double getMaxRadiusSq() const;
	double getMaxRadiusSq(Vec3f reference) const;
	Vec3f getNormalVecOfTriangle(Triangle triangle) const;

	bool intersects(const Shape& other, Vec3f& intersection, Vec3f& exitVector, const Vec3& centerConnection) const;
	bool intersectsTransformed(const Shape& other, const CFramef& relativeCFrame, Vec3f& intersection, Vec3f& exitVector) const;
	int furthestIndexInDirection(const Vec3f& direction) const;
	Vec3f furthestInDirection(const Vec3f& direction) const;

	inline Vec3f operator[](int index) const {
		return this->vertices[index >> 3][index & 7];
	}

	inline TriangleIter iterTriangles() const { 
		return TriangleIter { triangles.get(), triangleCount };
	};
	inline ShapeVecIterFactory iterVertices() const;
};

struct ShapeVecIter {
	const Shape& shape;
	int index;
	Vec3f operator*() const {
		return shape.operator[](index);
	}
	int operator++() {
		return ++index;
	}
	bool operator!=(const ShapeVecIter& other) const {
		return index != other.index;
	}
};
struct ShapeVecIterFactory {
	const Shape& shape;
	int size;
	inline ShapeVecIter begin() const {
		return ShapeVecIter{ shape, 0 };
	};
	inline ShapeVecIter end() const {
		return ShapeVecIter{ shape, size };
	};
};
inline ShapeVecIterFactory Shape::iterVertices() const {
	return ShapeVecIterFactory{ *this, vertexCount };
};
