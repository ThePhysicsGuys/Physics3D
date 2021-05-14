#pragma once

#include "../math/linalg/vec.h"
#include "../math/cframe.h"
#include "../math/boundingBox.h"
#include "../datastructures/alignedPtr.h"
#include "../datastructures/iteratorFactory.h"

namespace P3D {
struct Triangle {
	union {
		struct {
			int firstIndex, secondIndex, thirdIndex;
		};
		int indexes[3];
	};

	[[nodiscard]] bool sharesEdgeWith(Triangle other) const;
	[[nodiscard]] Triangle rightShift() const { return Triangle{thirdIndex, firstIndex, secondIndex}; }
	[[nodiscard]] Triangle leftShift() const { return Triangle{secondIndex, thirdIndex, firstIndex}; }
	Triangle operator~() const { return Triangle{firstIndex, thirdIndex, secondIndex}; }
	bool operator==(const Triangle& other) const;
	int& operator[](int i) { return indexes[i]; }
	const int& operator[](int i) const { return indexes[i]; }
};

struct ShapeVertexIter {
	float* curVertex;
	size_t offset;
	Vec3f operator*() const {
		return Vec3f{*curVertex, *(curVertex + offset), *(curVertex + 2 * offset)};
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
		return Triangle{*curTriangle, *(curTriangle + offset), *(curTriangle + 2 * offset)};
	}
	void operator++() {
		++curTriangle;
	}
	bool operator!=(const ShapeTriangleIter& other) const {
		return curTriangle != other.curTriangle;
	}
	bool operator==(const ShapeTriangleIter& other) const {
		return curTriangle == other.curTriangle;
	}
};

class TriangleMesh;

class MeshPrototype {
protected:
	UniqueAlignedPointer<float> vertices;
	UniqueAlignedPointer<int> triangles;
public:
	int vertexCount;
	int triangleCount;
	MeshPrototype();
	MeshPrototype(int vertexCount, int triangleCount);
	MeshPrototype(int vertexCount, int triangleCount, UniqueAlignedPointer<int>&& triangles);
	MeshPrototype(int vertexCount, int triangleCount, UniqueAlignedPointer<float>&& vertices, UniqueAlignedPointer<int>&& triangles);

	MeshPrototype(const MeshPrototype& mesh);
	MeshPrototype& operator=(const MeshPrototype& mesh);
	MeshPrototype(MeshPrototype&&) noexcept = default;
	MeshPrototype& operator=(MeshPrototype&&) noexcept = default;

	[[nodiscard]] Vec3f getVertex(int index) const;
	[[nodiscard]] Triangle getTriangle(int index) const;
};

class EditableMesh : public MeshPrototype {
public:
	EditableMesh(int vertexCount, int triangleCount);
	EditableMesh(int vertexCount, int triangleCount, const UniqueAlignedPointer<int>& triangles);
	EditableMesh(int vertexCount, int triangleCount, UniqueAlignedPointer<int>&& triangles);

	explicit EditableMesh(const MeshPrototype& mesh);
	explicit EditableMesh(MeshPrototype&&) noexcept;

	void setVertex(int index, Vec3f vertex);
	void setVertex(int index, float x, float y, float z);
	void setTriangle(int index, Triangle triangle);
	void setTriangle(int index, int a, int b, int c);
};

class TriangleMesh : public MeshPrototype {
protected:
	TriangleMesh(UniqueAlignedPointer<float>&& vertices, UniqueAlignedPointer<int>&& triangles, int vertexCount, int triangleCount);
public:

	TriangleMesh() = default;
	TriangleMesh(int vertexCount, int triangleCount, const Vec3f* vertices, const Triangle* triangles);
	TriangleMesh(TriangleMesh&&) noexcept = default;
	TriangleMesh& operator=(TriangleMesh&&) noexcept = default;
	TriangleMesh(const TriangleMesh&) = default;
	TriangleMesh& operator=(const TriangleMesh&) = default;

	explicit TriangleMesh(MeshPrototype&& mesh) noexcept;
	explicit TriangleMesh(const MeshPrototype& mesh);

	[[nodiscard]] IteratorFactory<ShapeVertexIter> iterVertices() const;
	[[nodiscard]] IteratorFactory<ShapeTriangleIter> iterTriangles() const;

	void getTriangles(Triangle* triangleBuf) const;
	void getVertices(Vec3f* vertexBuf) const;

	[[nodiscard]] TriangleMesh translated(Vec3f offset) const;
	[[nodiscard]] TriangleMesh rotated(Rotationf rotation) const;
	[[nodiscard]] TriangleMesh localToGlobal(CFramef frame) const;
	[[nodiscard]] TriangleMesh globalToLocal(CFramef frame) const;
	[[nodiscard]] TriangleMesh scaled(float scaleX, float scaleY, float scaleZ) const;
	[[nodiscard]] TriangleMesh scaled(DiagonalMat3f scale) const;
	[[nodiscard]] TriangleMesh translatedAndScaled(Vec3f translation, DiagonalMat3f scale) const;

	[[nodiscard]] Vec3f getNormalVecOfTriangle(Triangle triangle) const;

	[[nodiscard]] CircumscribingSphere getCircumscribingSphere() const;
	[[nodiscard]] double getMaxRadius() const;
	[[nodiscard]] double getMaxRadius(Vec3f reference) const;
	[[nodiscard]] double getMaxRadiusSq() const;
	[[nodiscard]] double getMaxRadiusSq(Vec3f reference) const;
	[[nodiscard]] double getScaledMaxRadius(DiagonalMat3 scale) const;
	[[nodiscard]] double getScaledMaxRadiusSq(DiagonalMat3 scale) const;


	[[nodiscard]] BoundingBox getBoundsFallback() const;
	[[nodiscard]] BoundingBox getBoundsFallback(const Mat3f& referenceFrame) const;
	[[nodiscard]] int furthestIndexInDirectionFallback(const Vec3f& direction) const;
	[[nodiscard]] Vec3f furthestInDirectionFallback(const Vec3f& direction) const;

	[[nodiscard]] BoundingBox getBoundsSSE() const;
	[[nodiscard]] BoundingBox getBoundsSSE(const Mat3f& referenceFrame) const;
	[[nodiscard]] int furthestIndexInDirectionSSE(const Vec3f& direction) const;
	[[nodiscard]] Vec3f furthestInDirectionSSE(const Vec3f& direction) const;

	[[nodiscard]] int furthestIndexInDirectionSSE4(const Vec3f& direction) const;
	[[nodiscard]] Vec3f furthestInDirectionSSE4(const Vec3f& direction) const;

	[[nodiscard]] BoundingBox getBoundsAVX() const;
	[[nodiscard]] BoundingBox getBoundsAVX(const Mat3f& referenceFrame) const;
	[[nodiscard]] int furthestIndexInDirectionAVX(const Vec3f& direction) const;
	[[nodiscard]] Vec3f furthestInDirectionAVX(const Vec3f& direction) const;

	[[nodiscard]] BoundingBox getBounds() const;
	[[nodiscard]] BoundingBox getBounds(const Mat3f& referenceFrame) const;
	[[nodiscard]] int furthestIndexInDirection(const Vec3f& direction) const;
	[[nodiscard]] Vec3f furthestInDirection(const Vec3f& direction) const;

	[[nodiscard]] double getIntersectionDistance(const Vec3& origin, const Vec3& direction) const;
};

TriangleMesh stripUnusedVertices(const Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount);
};