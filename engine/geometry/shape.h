#pragma once

#include "../math/vec3.h"
#include "../math/mat3.h"
#include "../math/cframe.h"
#include <memory>

struct Triangle;
struct Shape;
struct ShapeVecIter;
struct ShapeVecIterFactory;

#include "boundingBox.h"

struct Sphere {
	Vec3 origin;
	double radius;
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



struct ParallelVec3;
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
	ParallelVec3* vertices;
public:
	const Triangle* triangles;
	int vertexCount;
	int triangleCount;

	Shape() : vertices(nullptr), triangles(nullptr), vertexCount(0), triangleCount(0) {};
	Shape(Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount);

	Shape translated(Vec3f offset, Vec3f* newVecBuf) const;
	Shape rotated(RotMat3f rotation, Vec3f* newVecBuf) const;
	Shape localToGlobal(CFramef frame, Vec3f* newVecBuf) const;
	Shape globalToLocal(CFramef frame, Vec3f* newVecBuf) const;

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
	Sphere getCircumscribedSphere() const;
	void getCircumscribedEllipsoid() const;
	double getMaxRadius() const;
	Vec3f getNormalVecOfTriangle(Triangle triangle) const;

	bool intersects(const Shape& other, Vec3f& intersection, Vec3f& exitVector, const Vec3& centerConnection) const;
	bool intersectsTransformed(const Shape& other, const CFramef& relativeCFrame, Vec3f& intersection, Vec3f& exitVector) const;
	int furthestIndexInDirection(const Vec3f& direction) const;
	Vec3f furthestInDirection(const Vec3f& direction) const;

	Vec3f operator[](int index) const;

	inline TriangleIter iterTriangles() const { 
		return TriangleIter { triangles, triangleCount };
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
