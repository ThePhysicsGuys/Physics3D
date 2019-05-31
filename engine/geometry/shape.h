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

#ifdef __AVX__
#include "../datastructures/parallelVector.h"
#endif

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
#ifdef __AVX__
	AOSOAVec3fBuf vertices;
#else
	Vec3f* vertices;
#endif
public:
	std::shared_ptr<Vec3f> normals;
	std::shared_ptr<Vec2f> uvs;
	const Triangle* triangles;
	int vertexCount;
	int triangleCount;

	Shape();
	Shape(Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount);
	Shape(Vec3f* vertices, Vec3f* normals, const Triangle* triangles, int vertexCount, int triangleCount);
	Shape(Vec3f* vertices, Vec3f* normals, Vec2f* uvs, const Triangle* triangles, int vertexCount, int triangleCount);
	Shape(Vec3f* vertices, Vec2f* uvs, const Triangle* triangles, int vertexCount, int triangleCount);

	Shape translated(Vec3f offset, Vec3f* newVecBuf) const;
	Shape rotated(RotMat3f rotation, Vec3f* newVecBuf) const;
	Shape localToGlobal(CFramef frame, Vec3f* newVecBuf) const;
	Shape globalToLocal(CFramef frame, Vec3f* newVecBuf) const;

	bool isValid() const;
	bool containsPoint(Vec3f point) const;
	float getIntersectionDistance(Vec3f origin, Vec3f direction) const;
	double getVolume() const;
	NormalizedShape normalized(Vec3f* vecBuf, Vec3f* normalBuf, CFramef& backTransformation) const;
	CenteredShape centered(Vec3f* vecBuf, Vec3& backOffset) const;

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

	inline const Vec3f& operator[](int index) const { return vertices[index]; }
	inline Vec3f& operator[](int index) { return vertices[index]; }

	inline TriangleIter iterTriangles() const { 
		return TriangleIter { triangles, triangleCount };
	};
#ifdef __AVX__
	inline ParallelVecIterFactory iterVertices() const {
		return ParallelVecIterFactory{vertices, vertexCount};
	};
#else
	inline VertexIterFactory iterVertices() const {
		return VertexIterFactory{ vertices, vertexCount };
	};
#endif
};

struct CenteredShape : public Shape {
	friend struct Shape;
	CenteredShape() : Shape() {}
	CenteredShape(Vec3f * vertices, const Triangle * triangles, int vertexCount, int triangleCount);
	CenteredShape(Vec3f * vertices, Vec3f * normals, Vec2f * uvs, const Triangle * triangles, int vertexCount, int triangleCount);
private:
	CenteredShape(Vec3f * vertices, const Triangle * triangles, int vertexCount, int triangleCount, Vec3& offset);
};

struct NormalizedShape : public CenteredShape {
	friend struct Shape;
	NormalizedShape() : CenteredShape() {}
	NormalizedShape(Vec3f * vertices, const Triangle * triangles, int vertexCount, int triangleCount);
	NormalizedShape(Vec3f * vertices, Vec3f * normals, Vec2f * uvs, const Triangle * triangles, int vertexCount, int triangleCount);
private:
	NormalizedShape(Vec3f * vertices, const Triangle* triangles, int vertexCount, int triangleCount, CFramef& transformation);
};
