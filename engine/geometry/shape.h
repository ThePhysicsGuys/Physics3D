#pragma once

struct Triangle;
struct Shape;
struct NormalizedShape;

#include "../math/vec3.h"
#include "../math/vec4.h"
#include "../math/mat3.h"
#include "../math/cframe.h"
#include "boundingBox.h"

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

	struct VertexIter {
		Vec3* first;
		int size;
		inline Vec3* begin() const {
			return first; 
		};
		inline Vec3* end() const { 
			return first + size;
		};
	};

	Vec3* vertices;
	Vec3* normals;
	const Triangle* triangles;
	int vCount;
	int tCount;

	Shape();
	Shape(Vec3* vertices, const Triangle* triangles, int vertexCount, int triangleCount);
	Shape(Vec3* vertices, Vec3* normals, const Triangle* triangles, int vertexCount, int triangleCount);

	Shape translated(Vec3 offset, Vec3* newVecBuf) const;
	Shape rotated(RotMat3 rotation, Vec3* newVecBuf) const;
	Shape localToGlobal(CFrame frame, Vec3* newVecBuf) const;
	Shape globalToLocal(CFrame frame, Vec3* newVecBuf) const;

	bool isValid() const;
	bool containsPoint(Vec3 point) const;
	double getIntersectionDistance(Vec3 origin, Vec3 direction);
	double getVolume() const;
	NormalizedShape normalized(Vec3* vecBuf, CFrame& backTransformation) const;
	CFrame getInertialEigenVectors() const;
	BoundingBox getBounds() const;
	Vec3* getNormals() const;
	Vec3 getCenterOfMass() const;
	Mat3 getInertia() const;
	Mat3 getInertia(Vec3 reference) const;
	Mat3 getInertia(Mat3 reference) const;
	Mat3 getInertia(CFrame reference) const;
	Vec4 getCircumscribedSphere() const;
	Vec3 getNormalVecOfTriangle(Triangle triangle) const;

	inline TriangleIter iterTriangles() const { 
		return TriangleIter { triangles, tCount };
	};
	inline VertexIter iterVertices() const { 
		return VertexIter { vertices, vCount }; 
	};

};

struct NormalizedShape : public Shape {
	friend struct Shape;
	NormalizedShape() : Shape() {}
	NormalizedShape(Vec3* vertices, const Triangle* triangles, int vertexCount, int triangleCount);
private:
	NormalizedShape(Vec3* vertices, const Triangle* triangles, int vertexCount, int triangleCount, CFrame& transformation);
};
