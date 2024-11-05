#include "triangleMesh.h"

#include "../misc/validityHelper.h"
#include "../misc/cpuid.h"

#include <cstdio>
#include <math.h>
#include <vector>
#include <set>
#include <cmath>
#include <string.h>

namespace P3D {
#pragma region bufManagement
static size_t getOffset(size_t size) {
        return (size + 7) & 0xFFFFFFFFFFFFFFF8;
 

}
static UniqueAlignedPointer<float> createParallelVecBuf(size_t size) {
	return UniqueAlignedPointer<float>(getOffset(size) * 3, 32);
}
static UniqueAlignedPointer<int> createParallelTriangleBuf(size_t size) {
	return UniqueAlignedPointer<int>(getOffset(size) * 3, 32);
}

template<typename T>
static UniqueAlignedPointer<T> copy(const UniqueAlignedPointer<T>& buf, size_t size) {
	size_t totalBufSize = getOffset(size) * 3;
	UniqueAlignedPointer<T> result(totalBufSize, 32);


	memcpy(result.get(), buf, sizeof(T) * totalBufSize);


	return result;
}

template<typename T>
static void fixFinalBlock(T* buf, size_t size) {
	
	unsigned int totalValues = getOffset(size) * 3;
	unsigned int unusableValues_n = ((getOffset(size) * 3 - size * 3) / 3);
	
	unsigned int offset = totalValues -  unusableValues_n;

	T* xValues = buf + offset - 8 * 2;
	T* yValues = buf + offset - 8;
	T* zValues = buf + offset;

	unsigned int index = 0;

	for(size_t i = index; i < unusableValues_n; i++) {
		xValues[i] = xValues[-1];
		yValues[i] = yValues[-1];
        zValues[i] = zValues[-1];
	}
}
#pragma endregion

#pragma region triangle
bool Triangle::sharesEdgeWith(Triangle other) const {
	return firstIndex == other.secondIndex && secondIndex == other.firstIndex ||
		firstIndex == other.thirdIndex && secondIndex == other.secondIndex ||
		firstIndex == other.firstIndex && secondIndex == other.thirdIndex ||

		secondIndex == other.secondIndex && thirdIndex == other.firstIndex ||
		secondIndex == other.thirdIndex && thirdIndex == other.secondIndex ||
		secondIndex == other.firstIndex && thirdIndex == other.thirdIndex ||

		thirdIndex == other.secondIndex && firstIndex == other.firstIndex ||
		thirdIndex == other.thirdIndex && firstIndex == other.secondIndex ||
		thirdIndex == other.firstIndex && firstIndex == other.thirdIndex;
}

bool Triangle::operator==(const Triangle& other) const {
	return firstIndex == other.firstIndex && secondIndex == other.secondIndex && thirdIndex == other.thirdIndex ||
		firstIndex == other.secondIndex && secondIndex == other.thirdIndex && thirdIndex == other.firstIndex ||
		firstIndex == other.thirdIndex && secondIndex == other.firstIndex && thirdIndex == other.secondIndex;
}
#pragma endregion

#pragma region MeshPrototype
MeshPrototype::MeshPrototype(const MeshPrototype& mesh) :
	vertices(copy(mesh.vertices, mesh.vertexCount)),
	triangles(copy(mesh.triangles, mesh.triangleCount)),
	vertexCount(mesh.vertexCount),
	triangleCount(mesh.triangleCount) {

}

MeshPrototype& MeshPrototype::operator=(const MeshPrototype& mesh) {
	this->vertices = copy(mesh.vertices, mesh.vertexCount);
	this->triangles = copy(mesh.triangles, mesh.triangleCount);
	this->vertexCount = mesh.vertexCount;
	this->triangleCount = mesh.triangleCount;

	return *this;
}

MeshPrototype::MeshPrototype() :
	vertices(),
	triangles(),
	vertexCount(0),
	triangleCount(0) {}

MeshPrototype::MeshPrototype(int vertexCount, int triangleCount) :
    vertices(getOffset(vertexCount) * 3, 32),
	triangles(getOffset(triangleCount) * 3, 32),
	vertexCount(vertexCount),
	triangleCount(triangleCount) {}

MeshPrototype::MeshPrototype(int vertexCount, int triangleCount, UniqueAlignedPointer<int>&& triangles) :
	vertices(getOffset(vertexCount) * 3, 32),
	triangles(std::move(triangles)),
	vertexCount(vertexCount),
	triangleCount(triangleCount) {}

MeshPrototype::MeshPrototype(int vertexCount, int triangleCount, UniqueAlignedPointer<float>&& vertices, UniqueAlignedPointer<int>&& triangles) :
	vertices(std::move(vertices)),
	triangles(std::move(triangles)),
	vertexCount(vertexCount),
	triangleCount(triangleCount) {}

Vec3f MeshPrototype::getVertex(int index) const {
	// assert(index >= 0 && index < vertexCount);
	size_t currect_index = (index / BLOCK_WIDTH) * BLOCK_WIDTH * 2 + index;
	return Vec3f(this->vertices[currect_index], this->vertices[currect_index + BLOCK_WIDTH], this->vertices[currect_index + BLOCK_WIDTH * 2]);
}

Triangle MeshPrototype::getTriangle(int index) const {
	assert(index >= 0 && index < triangleCount);
	size_t currect_index = (index / BLOCK_WIDTH) * BLOCK_WIDTH * 2 + index;
	return Triangle{triangles[currect_index], triangles[currect_index + BLOCK_WIDTH], triangles[currect_index + BLOCK_WIDTH * 2]};
}
#pragma endregion

#pragma region EditableMesh
EditableMesh::EditableMesh(int vertexCount, int triangleCount) :
	MeshPrototype(vertexCount, triangleCount) {}
EditableMesh::EditableMesh(int vertexCount, int triangleCount, const UniqueAlignedPointer<int>& triangles) :
	MeshPrototype(vertexCount, triangleCount, copy(triangles, triangleCount)) {}
EditableMesh::EditableMesh(int vertexCount, int triangleCount, UniqueAlignedPointer<int>&& triangles) :
	MeshPrototype(vertexCount, triangleCount, std::move(triangles)) {}

EditableMesh::EditableMesh(const MeshPrototype& mesh) : MeshPrototype(mesh) {}
EditableMesh::EditableMesh(MeshPrototype&& mesh) noexcept : MeshPrototype(std::move(mesh)) {}

void EditableMesh::setVertex(int index, Vec3f newVertex) {
	assert(index >= 0 && index < vertexCount);
	size_t correct_index = (index / BLOCK_WIDTH) * BLOCK_WIDTH * 2 + index;
	this->vertices[correct_index] = newVertex.x;
	this->vertices[correct_index + BLOCK_WIDTH] = newVertex.y;
	this->vertices[correct_index + 2 * BLOCK_WIDTH] = newVertex.z;
}
void EditableMesh::setVertex(int index, float x, float y, float z) {
	assert(index >= 0 && index < vertexCount);
	size_t correct_index = (index / BLOCK_WIDTH) * BLOCK_WIDTH * 2 + index;
	this->vertices[correct_index] = x;
	this->vertices[correct_index + BLOCK_WIDTH] = y;
	this->vertices[correct_index + 2 * BLOCK_WIDTH] = z;
}
void EditableMesh::setTriangle(int index, Triangle newTriangle) {
	assert(index >= 0 && index < triangleCount);
	assert(isValidTriangle(newTriangle, vertexCount));

	size_t correct_index = (index / BLOCK_WIDTH) * BLOCK_WIDTH * 2 + index;
	this->triangles[correct_index] = newTriangle.firstIndex;
	this->triangles[correct_index + BLOCK_WIDTH] = newTriangle.secondIndex;
	this->triangles[correct_index + 2 * BLOCK_WIDTH] = newTriangle.thirdIndex;
}
void EditableMesh::setTriangle(int index, int a, int b, int c) {
	assert(index >= 0 && index < triangleCount);
	assert(isValidTriangle(Triangle{a,b,c}, vertexCount));

	size_t correct_index = (index / BLOCK_WIDTH) * BLOCK_WIDTH * 2 + index;
	this->triangles[correct_index] = a;
	this->triangles[correct_index + BLOCK_WIDTH] = b;
	this->triangles[correct_index + 2 * BLOCK_WIDTH] = c;
}
#pragma endregion

#pragma region TriangleMesh
TriangleMesh::TriangleMesh(UniqueAlignedPointer<float>&& vertices, UniqueAlignedPointer<int>&& triangles, int vertexCount, int triangleCount) :
	MeshPrototype(vertexCount, triangleCount, std::move(vertices), std::move(triangles)) {
	assert(isValid(*this));
}

TriangleMesh::TriangleMesh(int vertexCount, int triangleCount, const Vec3f* vertices, const Triangle* triangles) :
	MeshPrototype(vertexCount, triangleCount) {

    
	float* xValues = this->vertices.get();
	float* yValues = xValues + BLOCK_WIDTH;
	float* zValues = yValues + BLOCK_WIDTH;

	unsigned int index = 0;
	for(size_t i = 0; i < vertexCount; i++) {
	    xValues[index] = vertices[i].x;
	    yValues[index] = vertices[i].y;
	    zValues[index] = vertices[i].z;
	    index++;
		if((index % BLOCK_WIDTH) == 0)
	    	index += BLOCK_WIDTH * 2;
	    
	}

	if((index % BLOCK_WIDTH) == 0)
		index -= BLOCK_WIDTH * 2;

	unsigned int lastIndex = index - 1;
	unsigned int sizeLeft = (getOffset(vertexCount) * 3 - vertexCount * 3) / 3;
    for(unsigned int i = index; i < index + sizeLeft; i++){
	  xValues[i] = xValues[lastIndex];
	  yValues[i] = yValues[lastIndex];
	  zValues[i] = zValues[lastIndex];
    }
	
	
	
	int* aValues = this->triangles.get();
	int* bValues = aValues + BLOCK_WIDTH;
	int* cValues = bValues + BLOCK_WIDTH;

	index = 0;
	for(size_t i = 0; i < triangleCount; i++) {
 		aValues[index] = triangles[i].firstIndex;
		bValues[index] = triangles[i].secondIndex;
		cValues[index] = triangles[i].thirdIndex;
		index++;
		if((index % BLOCK_WIDTH) == 0)
			index += BLOCK_WIDTH * 2;
    }

	if((index % BLOCK_WIDTH) == 0)
		index -= BLOCK_WIDTH * 2;
	
	lastIndex = index - 1;
	sizeLeft =  (getOffset(triangleCount) * 3 - triangleCount * 3) / 3;
	
	for(unsigned int i=index; i < index + sizeLeft; i++){
	  aValues[i] = aValues[lastIndex];
	  bValues[i] = bValues[lastIndex];
	  cValues[i] = cValues[lastIndex];
	}
		
    
	assert(isValid(*this));
}

TriangleMesh::TriangleMesh(const MeshPrototype& mesh) :
	MeshPrototype(mesh) {
	fixFinalBlock(this->vertices.get(), vertexCount);
	fixFinalBlock(this->triangles.get(), triangleCount);
	assert(isValid(*this));
}

TriangleMesh::TriangleMesh(MeshPrototype&& mesh) noexcept :
	MeshPrototype(std::move(mesh)) {
	fixFinalBlock(this->vertices.get(), vertexCount);
	fixFinalBlock(this->triangles.get(), triangleCount);
	assert(isValid(*this));
}

IteratorFactory<ShapeVertexIter> TriangleMesh::iterVertices() const {

	unsigned int totalVertices = getOffset(vertexCount) * 3;
	unsigned int unusableVertices_n = ((getOffset(vertexCount) * 3 - vertexCount * 3) / 3);
	bool isMulOfBlockWidth = (vertexCount % BLOCK_WIDTH == 0);
  return IteratorFactory<ShapeVertexIter>(ShapeVertexIter{vertices, 0},
					  ShapeVertexIter{&vertices[totalVertices - unusableVertices_n - BLOCK_WIDTH * 2 + BLOCK_WIDTH * 2 * isMulOfBlockWidth], 0});
}
IteratorFactory<ShapeTriangleIter> TriangleMesh::iterTriangles() const {

	unsigned int totalTriangles = getOffset(triangleCount) * 3;
	unsigned int unusableTriangles_n = ((getOffset(triangleCount) * 3 - triangleCount * 3) / 3);
	bool isMulOfBlockWidth = (triangleCount % BLOCK_WIDTH == 0);
  return IteratorFactory<ShapeTriangleIter>(ShapeTriangleIter{triangles, 0},
					    ShapeTriangleIter{&triangles[totalTriangles - unusableTriangles_n - BLOCK_WIDTH * 2 +
						 BLOCK_WIDTH * 2 * isMulOfBlockWidth], 0});
}

void TriangleMesh::getTriangles(Triangle* triangleBuf) const {
	size_t i = 0;
	for(Triangle triangle : iterTriangles()) {
		triangleBuf[i++] = triangle;
	}
}
void TriangleMesh::getVertices(Vec3f* vertexBuf) const {
	size_t i = 0;
	for(Vec3f vertex : iterVertices()) {
		vertexBuf[i++] = vertex;
	}
}


TriangleMesh TriangleMesh::translated(Vec3f offset) const {
	EditableMesh result(this->vertexCount, this->triangleCount, this->triangles);
	for(int i = 0; i < this->vertexCount; i++) {
		result.setVertex(i, this->getVertex(i) + offset);
	}
	return TriangleMesh(std::move(result));
}

TriangleMesh TriangleMesh::rotated(Rotationf rotation) const {
	EditableMesh result(this->vertexCount, this->triangleCount, this->triangles);
	for(int i = 0; i < this->vertexCount; i++) {
		result.setVertex(i, rotation * this->getVertex(i));
	}
	return TriangleMesh(std::move(result));
}

TriangleMesh TriangleMesh::localToGlobal(CFramef frame) const {
	EditableMesh result(this->vertexCount, this->triangleCount, this->triangles);
	for(int i = 0; i < this->vertexCount; i++) {
		result.setVertex(i, frame.localToGlobal(this->getVertex(i)));
	}
	return TriangleMesh(std::move(result));
}

TriangleMesh TriangleMesh::globalToLocal(CFramef frame) const {
	EditableMesh result(this->vertexCount, this->triangleCount, this->triangles);
	for(int i = 0; i < this->vertexCount; i++) {
		result.setVertex(i, frame.globalToLocal(this->getVertex(i)));
	}
	return TriangleMesh(std::move(result));
}
TriangleMesh TriangleMesh::scaled(float scaleX, float scaleY, float scaleZ) const {
	EditableMesh result(this->vertexCount, this->triangleCount, this->triangles);
	for(int i = 0; i < this->vertexCount; i++) {
		Vec3f v = this->getVertex(i);
		result.setVertex(i, Vec3f(scaleX * v.x, scaleY * v.y, scaleZ * v.z));
	}
	return TriangleMesh(std::move(result));
}
TriangleMesh TriangleMesh::scaled(DiagonalMat3f scale) const {
	return scaled(scale[0], scale[1], scale[2]);
}

TriangleMesh TriangleMesh::translatedAndScaled(Vec3f translation, DiagonalMat3f scale) const {
	EditableMesh result(this->vertexCount, this->triangleCount, this->triangles);
	for(int i = 0; i < this->vertexCount; i++) {
		Vec3f cur = this->getVertex(i);
		result.setVertex(i, scale * (cur + translation));
	}
	return TriangleMesh(std::move(result));
}


Vec3f TriangleMesh::getNormalVecOfTriangle(Triangle triangle) const {
	Vec3f v0 = this->getVertex(triangle.firstIndex);
	return (this->getVertex(triangle.secondIndex) - v0) % (this->getVertex(triangle.thirdIndex) - v0);
}

void TriangleMesh::computeNormals(Vec3f* buffer) const {
	// TODO parallelize
	for (Triangle triangle : iterTriangles()) {
		Vec3f v0 = this->getVertex(triangle.firstIndex);
		Vec3f v1 = this->getVertex(triangle.secondIndex);
		Vec3f v2 = this->getVertex(triangle.thirdIndex);

		Vec3f D10 = normalize(v1 - v0);
		Vec3f D20 = normalize(v2 - v0);
		Vec3f D21 = normalize(v2 - v1);

		buffer[triangle.firstIndex] += D10 % D20;
		buffer[triangle.secondIndex] += D10 % D21;
		buffer[triangle.thirdIndex] += D20 % D21;
	}

	for (int i = 0; i < vertexCount; i++) {
		buffer[i] = normalize(buffer[i]);
	}
}


CircumscribingSphere TriangleMesh::getCircumscribingSphere() const {
	BoundingBox bounds = getBounds();
	Vec3 center = bounds.getCenter();
	double radius = getMaxRadius(center);
	return CircumscribingSphere{center, radius};
}

double TriangleMesh::getMaxRadiusSq() const {
	double bestDistSq = 0;
	for(Vec3f vertex : iterVertices()) {
		double distSq = lengthSquared(vertex);
		if(distSq > bestDistSq) {
			bestDistSq = distSq;
		}
	}
	return bestDistSq;
}

double TriangleMesh::getMaxRadiusSq(Vec3f reference) const {
	double bestDistSq = 0;
	for(Vec3f vertex : iterVertices()) {
		double distSq = lengthSquared(vertex - reference);
		if(distSq > bestDistSq) {
			bestDistSq = distSq;
		}
	}
	return bestDistSq;
}

double TriangleMesh::getMaxRadius() const {
	return sqrt(getMaxRadiusSq());
}

double TriangleMesh::getMaxRadius(Vec3f reference) const {
	return sqrt(getMaxRadiusSq(reference));
}


double TriangleMesh::getScaledMaxRadiusSq(DiagonalMat3 scale) const {
	double bestDistSq = 0;
	for(Vec3f vertex : iterVertices()) {
		double distSq = lengthSquared(scale * Vec3(vertex));
		if(distSq > bestDistSq) {
			bestDistSq = distSq;
		}
	}
	return bestDistSq;
}
double TriangleMesh::getScaledMaxRadius(DiagonalMat3 scale) const {
	return sqrt(getScaledMaxRadiusSq(scale));
}

double TriangleMesh::getIntersectionDistance(const Vec3& origin, const Vec3& direction) const {
	const double EPSILON = 0.0000001;
	double t = std::numeric_limits<double>::max();
	for(Triangle triangle : iterTriangles()) {
		Vec3 v0 = this->getVertex(triangle.firstIndex);
		Vec3 v1 = this->getVertex(triangle.secondIndex);
		Vec3 v2 = this->getVertex(triangle.thirdIndex);

		Vec3 edge1 = v1 - v0;
		Vec3 edge2 = v2 - v0;
		Vec3 h = direction % edge2;

		double a = edge1 * h;
		if(a > -EPSILON && a < EPSILON)
			continue;

		Vec3 s = origin - v0;
		double f = 1.0 / a;
		double u = f * (s * h);

		if(u < 0.0 || u > 1.0)
			continue;

		Vec3 q = s % edge1;
		double v = direction * f * q;

		if(v < 0.0 || u + v > 1.0)
			continue;

		double r = edge2 * f * q;
		if(r > EPSILON) {
			if(r < t)
				t = r;
		} else {
			//Log::debug("Line intersection but not a ray intersection");
			continue;
		}
	}

	return t;
}


TriangleMesh stripUnusedVertices(const Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount) {
	bool* vertexIsReferenced = new bool[vertexCount];
	for(int i = 0; i < vertexCount; i++) { vertexIsReferenced[i] = false; }
	for(int i = 0; i < triangleCount; i++) {
		Triangle t = triangles[i];
		vertexIsReferenced[t.firstIndex] = true;
		vertexIsReferenced[t.secondIndex] = true;
		vertexIsReferenced[t.thirdIndex] = true;
	}
	int totalUnReferencedCount = 0;
	for(int i = 0; i < vertexCount; i++) {
		if(vertexIsReferenced[i] == false) {
			totalUnReferencedCount++;
		}
	}
	int lastValidVertex = vertexCount - 1;
	while(vertexIsReferenced[lastValidVertex] == false) {
		lastValidVertex--;
	}
	std::vector<std::pair<int, int>> substitutions(totalUnReferencedCount); // substitute first with second in triangles
	EditableMesh result(vertexCount - totalUnReferencedCount, triangleCount);
	// fix vertices
	for(int i = 0; i <= lastValidVertex; i++) {
		if(vertexIsReferenced[i] == false) {
			result.setVertex(i, vertices[lastValidVertex]);
			substitutions.push_back(std::make_pair(lastValidVertex, i));
			do {
				lastValidVertex--;
			} while(vertexIsReferenced[lastValidVertex] == false && lastValidVertex > i); // the second condition is for the case that the first n vertices are all invalid, which would make lastValidVertex run off 0
		} else {
			result.setVertex(i, vertices[i]);
		}
	}
	delete[] vertexIsReferenced;
	for(int i = 0; i < triangleCount; i++) {
		Triangle t = triangles[i];
		for(std::pair<int, int>& sub : substitutions) {
			if(t.firstIndex == sub.first) t.firstIndex = sub.second;
			if(t.secondIndex == sub.first) t.secondIndex = sub.second;
			if(t.thirdIndex == sub.first) t.thirdIndex = sub.second;
		}
		result.setTriangle(i, t);
	}
	return TriangleMesh(std::move(result));
}

int TriangleMesh::furthestIndexInDirectionFallback(const Vec3f& direction) const {
	float bestDot = this->getVertex(0) * direction;
	int bestVertexIndex = 0;
	for(int i = 1; i < vertexCount; i++) {
		float newD = this->getVertex(i) * direction;
		if(newD > bestDot) {
			bestDot = newD;
			bestVertexIndex = i;
		}
	}

	return bestVertexIndex;
}

Vec3f TriangleMesh::furthestInDirectionFallback(const Vec3f& direction) const {
	float bestDot = this->getVertex(0) * direction;
	Vec3f bestVertex = this->getVertex(0);
	for(int i = 1; i < vertexCount; i++) {
		float newD = this->getVertex(i) * direction;
		if(newD > bestDot) {
			bestDot = newD;
			bestVertex = this->getVertex(i);
		}
	}

	return bestVertex;
}


BoundingBox TriangleMesh::getBoundsFallback() const {
	double xmin = this->getVertex(0).x, xmax = this->getVertex(0).x;
	double ymin = this->getVertex(0).y, ymax = this->getVertex(0).y;
	double zmin = this->getVertex(0).z, zmax = this->getVertex(0).z;

	for(int i = 1; i < vertexCount; i++) {
		const Vec3f current = this->getVertex(i);

		if(current.x < xmin) xmin = current.x;
		if(current.x > xmax) xmax = current.x;
		if(current.y < ymin) ymin = current.y;
		if(current.y > ymax) ymax = current.y;
		if(current.z < zmin) zmin = current.z;
		if(current.z > zmax) zmax = current.z;
	}

	return BoundingBox{xmin, ymin, zmin, xmax, ymax, zmax};
}

BoundingBox TriangleMesh::getBoundsFallback(const Mat3f& referenceFrame) const {
	Mat3f transp = referenceFrame.transpose();
	double xmax = (referenceFrame * this->furthestInDirection(transp * Vec3f(1, 0, 0))).x;
	double xmin = (referenceFrame * this->furthestInDirection(transp * Vec3f(-1, 0, 0))).x;
	double ymax = (referenceFrame * this->furthestInDirection(transp * Vec3f(0, 1, 0))).y;
	double ymin = (referenceFrame * this->furthestInDirection(transp * Vec3f(0, -1, 0))).y;
	double zmax = (referenceFrame * this->furthestInDirection(transp * Vec3f(0, 0, 1))).z;
	double zmin = (referenceFrame * this->furthestInDirection(transp * Vec3f(0, 0, -1))).z;

	return BoundingBox(xmin, ymin, zmin, xmax, ymax, zmax);
}

int TriangleMesh::furthestIndexInDirection(const Vec3f& direction) const {
	if(CPUIDCheck::hasTechnology(CPUIDCheck::AVX | CPUIDCheck::AVX2 | CPUIDCheck::FMA)) {
		return furthestIndexInDirectionAVX(direction);
	} else if(CPUIDCheck::hasTechnology(CPUIDCheck::SSE | CPUIDCheck::SSE2)) {
		if(CPUIDCheck::hasTechnology(CPUIDCheck::SSE4_1)) {
			return furthestIndexInDirectionSSE4(direction);
		} else {
			return furthestIndexInDirectionSSE(direction);
		}
	} else {
		return furthestIndexInDirectionFallback(direction);
	}
}

Vec3f TriangleMesh::furthestInDirection(const Vec3f& direction) const {
	if(CPUIDCheck::hasTechnology(CPUIDCheck::AVX | CPUIDCheck::AVX2 | CPUIDCheck::FMA)) {
		return furthestInDirectionAVX(direction);
	} else if(CPUIDCheck::hasTechnology(CPUIDCheck::SSE | CPUIDCheck::SSE2)) {
		if(CPUIDCheck::hasTechnology(CPUIDCheck::SSE4_1)) {
			return furthestInDirectionSSE4(direction);
		} else {
			return furthestInDirectionSSE(direction);
		}
	} else {
		return furthestInDirectionFallback(direction);
	}
}

BoundingBox TriangleMesh::getBounds() const {
	if(CPUIDCheck::hasTechnology(CPUIDCheck::AVX | CPUIDCheck::AVX2 | CPUIDCheck::FMA)) {
		return getBoundsAVX();
	} else if(CPUIDCheck::hasTechnology(CPUIDCheck::SSE | CPUIDCheck::SSE2)) {
		return getBoundsSSE();
	} else {
		return getBoundsFallback();
	}
}

BoundingBox TriangleMesh::getBounds(const Mat3f& referenceFrame) const {
	if(CPUIDCheck::hasTechnology(CPUIDCheck::AVX | CPUIDCheck::AVX2 | CPUIDCheck::FMA)) {
		return getBoundsAVX(referenceFrame);
	} else if(CPUIDCheck::hasTechnology(CPUIDCheck::SSE | CPUIDCheck::SSE2)) {
		return getBoundsSSE(referenceFrame);
	} else {
		return getBoundsFallback(referenceFrame);
	}
}

#pragma endregion
};
