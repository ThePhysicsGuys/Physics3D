#pragma once

#include "../math/linalg/vec.h"
#include "../math/cframe.h"
#include "boundingBox.h"
#include "../datastructures/alignedPtr.h"
#include "../datastructures/iteratorFactory.h"

struct Triangle {
	union {
		struct {
			int firstIndex, secondIndex, thirdIndex;
		};
		int indexes[3];
	};

	bool sharesEdgeWith(Triangle other) const;
	inline Triangle rightShift() const { return Triangle{thirdIndex, firstIndex, secondIndex}; }
	inline Triangle leftShift() const { return Triangle{secondIndex, thirdIndex, firstIndex}; }
	inline Triangle operator~() const { return Triangle{firstIndex, thirdIndex, secondIndex}; }
	bool operator==(const Triangle& other) const;
	inline int& operator[](int i) {return indexes[i];};
	inline const int& operator[](int i) const {return indexes[i];};
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
	inline Triangle operator*() const {
		return Triangle{*curTriangle, *(curTriangle + offset), *(curTriangle + 2 * offset)};
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

	Vec3f getVertex(int index) const;
	Triangle getTriangle(int index) const;
};

class EditableMesh : public MeshPrototype {
public:
	EditableMesh(int vertexCount, int triangleCount);
	EditableMesh(int vertexCount, int triangleCount, const UniqueAlignedPointer<int>& triangles);
	EditableMesh(int vertexCount, int triangleCount, UniqueAlignedPointer<int>&& triangles);

	explicit EditableMesh(const MeshPrototype& mesh);
	explicit EditableMesh(MeshPrototype&&) noexcept;

	void setVertex(int index, Vec3f vertex);
	void setTriangle(int index, Triangle triangle);
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
	
	IteratorFactory<ShapeVertexIter> iterVertices() const;
	IteratorFactory<ShapeTriangleIter> iterTriangles() const;

	void getTriangles(Triangle* triangleBuf) const;
	void getVertices(Vec3f* vertexBuf) const;

	TriangleMesh translated(Vec3f offset) const;
	TriangleMesh rotated(Rotationf rotation) const;
	TriangleMesh localToGlobal(CFramef frame) const;
	TriangleMesh globalToLocal(CFramef frame) const;
	TriangleMesh scaled(float scaleX, float scaleY, float scaleZ) const;
	TriangleMesh scaled(DiagonalMat3f scale) const;
	TriangleMesh translatedAndScaled(Vec3f translation, DiagonalMat3f scale) const;

	Vec3f getNormalVecOfTriangle(Triangle triangle) const;

	BoundingBox getBounds() const;
	BoundingBox getBounds(const Mat3f& referenceFrame) const;
	CircumscribingSphere getCircumscribingSphere() const;
	double getMaxRadius() const;
	double getMaxRadius(Vec3f reference) const;
	double getMaxRadiusSq() const;
	double getMaxRadiusSq(Vec3f reference) const;
	double getScaledMaxRadius(DiagonalMat3 scale) const;
	double getScaledMaxRadiusSq(DiagonalMat3 scale) const;

	int furthestIndexInDirection(const Vec3f& direction) const;
	Vec3f furthestInDirection(const Vec3f& direction) const;

	float getIntersectionDistance(Vec3f origin, Vec3f direction) const;
};
