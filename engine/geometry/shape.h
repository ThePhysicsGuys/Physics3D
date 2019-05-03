#pragma once

struct Triangle;
struct NormalizedShape;
struct CenteredShape;
struct Shape;

#include "../math/vec2.h"
#include "../math/vec3.h"
#include "../math/mat3.h"
#include "../math/cframe.h"
#include "boundingBox.h"
#include <memory>


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
	std::shared_ptr<Vec3> normals;
	std::shared_ptr<Vec2> uvs;
	const Triangle* triangles;
	int vertexCount;
	int triangleCount;

	Shape();
	Shape(Vec3* vertices, const Triangle* triangles, int vertexCount, int triangleCount);
	Shape(Vec3* vertices, Vec3* normals, const Triangle* triangles, int vertexCount, int triangleCount);
	Shape(Vec3* vertices, Vec3* normals, Vec2* uvs, const Triangle* triangles, int vertexCount, int triangleCount);
	Shape(Vec3* vertices, Vec2* uvs, const Triangle* triangles, int vertexCount, int triangleCount);

	Shape translated(Vec3 offset, Vec3* newVecBuf) const;
	Shape rotated(RotMat3 rotation, Vec3* newVecBuf) const;
	Shape localToGlobal(CFrame frame, Vec3* newVecBuf) const;
	Shape globalToLocal(CFrame frame, Vec3* newVecBuf) const;

	bool isValid() const;
	bool containsPoint(Vec3 point) const;
	double getIntersectionDistance(Vec3 origin, Vec3 direction);
	double getVolume() const;
	NormalizedShape normalized(Vec3* vecBuf, CFrame& backTransformation) const;
	CenteredShape centered(Vec3* vecBuf, Vec3& backOffset) const;

	CFrame getInertialEigenVectors() const;
	BoundingBox getBounds() const;
	void computeNormals(Vec3* buffer) const;
	Vec3 getCenterOfMass() const;
	Mat3 getInertia() const;
	Mat3 getInertia(Vec3 reference) const;
	Mat3 getInertia(Mat3 reference) const;
	Mat3 getInertia(CFrame reference) const;
	Sphere getCircumscribedSphere() const;
	double getMaxRadius() const;
	Vec3 getNormalVecOfTriangle(Triangle triangle) const;

	bool intersects(const Shape& other, Vec3& intersection, Vec3& exitVector, Vec3 centerConnection) const;
	bool intersectsTransformed(const Shape& other, const CFrame& relativeCFrame, Vec3& intersection, Vec3& exitVector) const;
	int furthestIndexInDirection(Vec3 direction) const;
	Vec3 furthestInDirection(Vec3 direction) const;

	inline TriangleIter iterTriangles() const { 
		return TriangleIter { triangles, triangleCount };
	};
	inline VertexIter iterVertices() const { 
		return VertexIter { vertices, vertexCount };
	};
};

struct CenteredShape : public Shape {
	friend struct Shape;
	CenteredShape() : Shape() {}
	CenteredShape(Vec3 * vertices, const Triangle * triangles, int vertexCount, int triangleCount);
	CenteredShape(Vec3 * vertices, Vec3 * normals, Vec2 * uvs, const Triangle * triangles, int vertexCount, int triangleCount);
private:
	CenteredShape(Vec3 * vertices, const Triangle * triangles, int vertexCount, int triangleCount, Vec3& offset);
};

struct NormalizedShape : public CenteredShape {
	friend struct Shape;
	NormalizedShape() : CenteredShape() {}
	NormalizedShape(Vec3 * vertices, const Triangle * triangles, int vertexCount, int triangleCount);
	NormalizedShape(Vec3 * vertices, Vec3 * normals, Vec2 * uvs, const Triangle * triangles, int vertexCount, int triangleCount);
private:
	NormalizedShape(Vec3* vertices, const Triangle* triangles, int vertexCount, int triangleCount, CFrame& transformation);
};
