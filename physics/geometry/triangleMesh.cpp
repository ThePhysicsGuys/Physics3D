#include "triangleMesh.h"

#include "../misc/validityHelper.h"

#include <stddef.h>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <set>
#include <math.h>

#pragma region bufManagement
inline static size_t getOffset(size_t size) {
	return (size + 7) & 0xFFFFFFFFFFFFFFF8;
}
inline static UniqueAlignedPointer<float> createParallelVecBuf(size_t size) {
	return UniqueAlignedPointer<float>(getOffset(size) * 3, 32);
}
inline static UniqueAlignedPointer<int> createParallelTriangleBuf(size_t size) {
	return UniqueAlignedPointer<int>(getOffset(size) * 3, 32);
}

template<typename T>
static UniqueAlignedPointer<T> copy(const UniqueAlignedPointer<T>& buf, size_t size) {
	size_t totalBufSize = getOffset(size) * 3;
	UniqueAlignedPointer<T> result(totalBufSize, 32);

	for(size_t i = 0; i < totalBufSize; i++) {
		result[i] = buf[i];
	}

	return result;
}

template<typename T>
static void fixFinalBlock(T* buf, size_t size) {
	size_t offset = getOffset(size);
	T* xValues = buf;
	T* yValues = buf + offset;
	T* zValues = buf + 2 * offset;

	for(size_t i = size; i < offset; i++) {
		xValues[i] = xValues[size - 1];
		yValues[i] = yValues[size - 1];
		zValues[i] = zValues[size - 1];
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
	assert(index >= 0 && index < vertexCount);
	size_t offset = getOffset(vertexCount);
	return Vec3f(this->vertices[index], this->vertices[index + offset], this->vertices[index + 2 * offset]);
}

Triangle MeshPrototype::getTriangle(int index) const {
	assert(index >= 0 && index < triangleCount);
	size_t offset = getOffset(triangleCount);
	return Triangle{triangles[index], triangles[index + offset], triangles[index + 2 * offset]};
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
	size_t offset = getOffset(vertexCount);
	this->vertices[index] = newVertex.x;
	this->vertices[index + offset] = newVertex.y;
	this->vertices[index + 2 * offset] = newVertex.z;
}
void EditableMesh::setVertex(int index, float x, float y, float z) {
	assert(index >= 0 && index < vertexCount);
	size_t offset = getOffset(vertexCount);
	this->vertices[index] = x;
	this->vertices[index + offset] = y;
	this->vertices[index + 2 * offset] = z;
}
void EditableMesh::setTriangle(int index, Triangle newTriangle) {
	assert(index >= 0 && index < triangleCount);
	assert(isValidTriangle(newTriangle, vertexCount));

	size_t offset = getOffset(triangleCount);
	this->triangles[index] = newTriangle.firstIndex;
	this->triangles[index + offset] = newTriangle.secondIndex;
	this->triangles[index + 2 * offset] = newTriangle.thirdIndex;
}
void EditableMesh::setTriangle(int index, int a, int b, int c) {
	assert(index >= 0 && index < triangleCount);
	assert(isValidTriangle(Triangle{a,b,c}, vertexCount));

	size_t offset = getOffset(triangleCount);
	this->triangles[index] = a;
	this->triangles[index + offset] = b;
	this->triangles[index + 2 * offset] = c;
}
#pragma endregion

#pragma region TriangleMesh
TriangleMesh::TriangleMesh(UniqueAlignedPointer<float>&& vertices, UniqueAlignedPointer<int>&& triangles, int vertexCount, int triangleCount) :
	MeshPrototype(vertexCount, triangleCount, std::move(vertices), std::move(triangles)) {
	assert(isValid(*this));
}

TriangleMesh::TriangleMesh(int vertexCount, int triangleCount, const Vec3f* vertices, const Triangle* triangles) :
	MeshPrototype(vertexCount, triangleCount) {

	size_t vertexOffset = getOffset(vertexCount);

	float* xValues = this->vertices.get();
	float* yValues = xValues + vertexOffset;
	float* zValues = yValues + vertexOffset;

	for(size_t i = 0; i < vertexCount; i++) {
		xValues[i] = vertices[i].x;
		yValues[i] = vertices[i].y;
		zValues[i] = vertices[i].z;
	}
	fixFinalBlock(this->vertices.get(), vertexCount);

	size_t triangleOffset = getOffset(triangleCount);

	int* aValues = this->triangles.get();
	int* bValues = aValues + triangleOffset;
	int* cValues = bValues + triangleOffset;

	for(size_t i = 0; i < triangleCount; i++) {
		aValues[i] = triangles[i].firstIndex;
		bValues[i] = triangles[i].secondIndex;
		cValues[i] = triangles[i].thirdIndex;
	}
	fixFinalBlock(this->triangles.get(), triangleCount);

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
	return IteratorFactory<ShapeVertexIter>(ShapeVertexIter{vertices, getOffset(vertexCount)}, ShapeVertexIter{vertices + vertexCount, getOffset(vertexCount)});
}
IteratorFactory<ShapeTriangleIter> TriangleMesh::iterTriangles() const {
	return IteratorFactory<ShapeTriangleIter>(ShapeTriangleIter{triangles, getOffset(triangleCount)}, ShapeTriangleIter{triangles + triangleCount, getOffset(triangleCount)});
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



CircumscribingSphere TriangleMesh::getCircumscribingSphere() const {
	BoundingBox bounds = getBounds();
	Vec3 center = Vec3(bounds.xmax + bounds.xmin, bounds.ymax + bounds.ymin, bounds.zmax + bounds.zmin) / 2.0;
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

float TriangleMesh::getIntersectionDistance(Vec3f origin, Vec3f direction) const {
	const float EPSILON = 0.0000001f;
	float t = INFINITY;
	for(Triangle triangle : iterTriangles()) {
		Vec3f v0 = this->getVertex(triangle.firstIndex);
		Vec3f v1 = this->getVertex(triangle.secondIndex);
		Vec3f v2 = this->getVertex(triangle.thirdIndex);

		Vec3f edge1, edge2, h, s, q;
		float a, f, u, v;

		edge1 = v1 - v0;
		edge2 = v2 - v0;

		h = direction % edge2;
		a = edge1 * h;

		if(a > -EPSILON && a < EPSILON) continue;

		f = 1.0f / a;
		s = origin - v0;
		u = f * (s * h);

		if(u < 0.0 || u > 1.0) continue;

		q = s % edge1;
		v = direction * f * q;

		if(v < 0.0f || u + v > 1.0f) continue;

		float r = edge2 * f * q;
		if(r > EPSILON) {
			if(r < t) t = r;
		} else {
			//Log::debug("Line intersection but not a ray intersection");
			continue;
		}
	}

	return t;
}


TriangleMesh stripUnusedVertices(const Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount) {
	bool* vertexIsReferenced = new bool[vertexCount];
	for(int i = 0; i < vertexCount; i++) {vertexIsReferenced[i] = false;}
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
		for(std::pair<int, int>& sub : substitutions){
			if(t.firstIndex == sub.first) t.firstIndex = sub.second;
			if(t.secondIndex == sub.first) t.secondIndex = sub.second;
			if(t.thirdIndex == sub.first) t.thirdIndex = sub.second;
		}
		result.setTriangle(i, t);
	}
	return TriangleMesh(std::move(result));
}


#ifdef __AVX2__
#include <immintrin.h>
#ifdef _MSC_VER
inline static uint32_t countZeros(uint32_t x) {
	unsigned long ret;
	_BitScanForward(&ret, x);
	return ( int) ret;
}
#else
inline static uint32_t countZeros(uint32_t x) {
	return __builtin_ctz(x);
}
#endif

#ifdef _MSC_VER
#define GET_AVX_ELEM(reg, index) reg.m256_f32[index]
#define GET_SSE_ELEM(reg, index) reg.mm[index]
#else
#define GET_AVX_ELEM(reg, index) reg[index]
#define GET_SSE_ELEM(reg, index) reg[index]
#endif

#define SWAP_2x2 0b01001110
#define SWAP_1x1 0b10110001

inline __m256i _mm256_blendv_epi32(__m256i a, __m256i b, __m256 mask) {
	return _mm256_castps_si256(
		_mm256_blendv_ps(
		_mm256_castsi256_ps(a),
		_mm256_castsi256_ps(b),
		mask
	)
	);
}

inline uint32_t mm256_extract_epi32_var_indx(__m256i vec, int i) {
	__m128i indx = _mm_cvtsi32_si128(i);
	__m256i val = _mm256_permutevar8x32_epi32(vec, _mm256_castsi128_si256(indx));
	return         _mm_cvtsi128_si32(_mm256_castsi256_si128(val));
}

int TriangleMesh::furthestIndexInDirection(const Vec3f& direction) const {
	size_t vertexCount = this->vertexCount;

	__m256 dx = _mm256_set1_ps(direction.x);
	__m256 dy = _mm256_set1_ps(direction.y);
	__m256 dz = _mm256_set1_ps(direction.z);

	size_t offset = getOffset(vertexCount);
	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	__m256 bestDot = _mm256_fmadd_ps(dz, _mm256_load_ps(zValues), _mm256_fmadd_ps(dy, _mm256_load_ps(yValues), _mm256_mul_ps(dx, _mm256_load_ps(xValues))));

	__m256i bestIndices = _mm256_set1_epi32(0);

	for(size_t blockI = 1; blockI < (vertexCount + 7) / 8; blockI++) {
		__m256i indices = _mm256_set1_epi32(int(blockI));

		__m256 dot = _mm256_fmadd_ps(dz, _mm256_load_ps(zValues + blockI * 8), _mm256_fmadd_ps(dy, _mm256_load_ps(yValues + blockI * 8), _mm256_mul_ps(dx, _mm256_load_ps(xValues + blockI * 8))));

		__m256 whichAreMax = _mm256_cmp_ps(dot, bestDot, _CMP_GT_OQ); // Greater than, false if dot == NaN
		bestDot = _mm256_blendv_ps(bestDot, dot, whichAreMax);
		bestIndices = _mm256_blendv_epi32(bestIndices, indices, whichAreMax); // TODO convert to _mm256_blendv_epi8
	}
	// find max of our 8 left candidates
	__m256 swap4x4 = _mm256_permute2f128_ps(bestDot, bestDot, 1);
	__m256 bestDotInternalMax = _mm256_max_ps(bestDot, swap4x4);
	__m256 swap2x2 = _mm256_permute_ps(bestDotInternalMax, SWAP_2x2);
	bestDotInternalMax = _mm256_max_ps(bestDotInternalMax, swap2x2);
	__m256 swap1x1 = _mm256_permute_ps(bestDotInternalMax, SWAP_1x1);
	bestDotInternalMax = _mm256_max_ps(bestDotInternalMax, swap1x1);

	__m256 compare = _mm256_cmp_ps(bestDotInternalMax, bestDot, _CMP_EQ_UQ);
	uint32_t mask = _mm256_movemask_ps(compare);

	assert(mask != 0);

	uint32_t index = countZeros(mask);
	uint32_t block = mm256_extract_epi32_var_indx(bestIndices, index);
	return block * 8 + index;
}

Vec3f TriangleMesh::furthestInDirection(const Vec3f& direction) const {
	size_t vertexCount = this->vertexCount;

	__m256 dx = _mm256_set1_ps(direction.x);
	__m256 dy = _mm256_set1_ps(direction.y);
	__m256 dz = _mm256_set1_ps(direction.z);

	size_t offset = getOffset(vertexCount);
	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	__m256 bestX = _mm256_load_ps(xValues);
	__m256 bestY = _mm256_load_ps(yValues);
	__m256 bestZ = _mm256_load_ps(zValues);

	__m256 bestDot = _mm256_fmadd_ps(dz, bestZ, _mm256_fmadd_ps(dy, bestY, _mm256_mul_ps(dx, bestX)));

	for(size_t blockI = 1; blockI < (vertexCount + 7) / 8; blockI++) {
		__m256i indices = _mm256_set1_epi32(int(blockI));

		__m256 xVal = _mm256_load_ps(xValues + blockI * 8);
		__m256 yVal = _mm256_load_ps(yValues + blockI * 8);
		__m256 zVal = _mm256_load_ps(zValues + blockI * 8);

		__m256 dot = _mm256_fmadd_ps(dz, zVal, _mm256_fmadd_ps(dy, yVal, _mm256_mul_ps(dx, xVal)));

		__m256 whichAreMax = _mm256_cmp_ps(dot, bestDot, _CMP_GT_OQ); // Greater than, false if dot == NaN
		bestDot = _mm256_blendv_ps(bestDot, dot, whichAreMax);
		bestX = _mm256_blendv_ps(bestX, xVal, whichAreMax);
		bestY = _mm256_blendv_ps(bestY, yVal, whichAreMax);
		bestZ = _mm256_blendv_ps(bestZ, zVal, whichAreMax);
	}

	// now we find the max of the remaining 8 elements
	__m256 swap4x4 = _mm256_permute2f128_ps(bestDot, bestDot, 1);
	__m256 bestDotInternalMax = _mm256_max_ps(bestDot, swap4x4);
	__m256 swap2x2 = _mm256_permute_ps(bestDotInternalMax, SWAP_2x2);
	bestDotInternalMax = _mm256_max_ps(bestDotInternalMax, swap2x2);
	__m256 swap1x1 = _mm256_permute_ps(bestDotInternalMax, SWAP_1x1);
	bestDotInternalMax = _mm256_max_ps(bestDotInternalMax, swap1x1);

	__m256 compare = _mm256_cmp_ps(bestDotInternalMax, bestDot, _CMP_EQ_UQ);
	uint32_t mask = _mm256_movemask_ps(compare);

	assert(mask != 0);

	uint32_t index = countZeros(mask);

	// a bug occurs here, when mask == 0 the resulting index is undefined

	return Vec3f(GET_AVX_ELEM(bestX, index), GET_AVX_ELEM(bestY, index), GET_AVX_ELEM(bestZ, index));
}

// compare the remaining 8 elements
BoundingBox toBounds(__m256 xMin, __m256 xMax, __m256 yMin, __m256 yMax, __m256 zMin, __m256 zMax) {
	// now we compare the remaining 8 elements
	__m256 xyMin = _mm256_min_ps(_mm256_permute2f128_ps(xMin, yMin, 0x20), _mm256_permute2f128_ps(xMin, yMin, 0x31));
	__m256 xyMax = _mm256_max_ps(_mm256_permute2f128_ps(xMax, yMax, 0x20), _mm256_permute2f128_ps(xMax, yMax, 0x31));
	zMin = _mm256_min_ps(zMin, _mm256_permute2f128_ps(zMin, zMin, 1));
	zMax = _mm256_max_ps(zMax, _mm256_permute2f128_ps(zMax, zMax, 1));

	xyMin = _mm256_min_ps(xyMin, _mm256_permute_ps(xyMin, SWAP_2x2));
	xyMax = _mm256_max_ps(xyMax, _mm256_permute_ps(xyMax, SWAP_2x2));

	zMin = _mm256_min_ps(zMin, _mm256_permute_ps(zMin, SWAP_2x2));
	zMax = _mm256_max_ps(zMax, _mm256_permute_ps(zMax, SWAP_2x2));


	__m256 zxzyMin = _mm256_blend_ps(xyMin, zMin, 0b00110011); // stored as xxyyzzzz
	zxzyMin = _mm256_min_ps(zxzyMin, _mm256_permute_ps(zxzyMin, SWAP_1x1));

	__m256 zxzyMax = _mm256_blend_ps(xyMax, zMax, 0b00110011);
	zxzyMax = _mm256_max_ps(zxzyMax, _mm256_permute_ps(zxzyMax, SWAP_1x1));
	// reg structure zzxxzzyy

	return BoundingBox{GET_AVX_ELEM(zxzyMin,2), GET_AVX_ELEM(zxzyMin, 6), GET_AVX_ELEM(zxzyMin, 0), GET_AVX_ELEM(zxzyMax, 2), GET_AVX_ELEM(zxzyMax, 6), GET_AVX_ELEM(zxzyMax, 0)};
}

BoundingBox TriangleMesh::getBounds() const {
	size_t vertexCount = this->vertexCount;

	size_t offset = getOffset(vertexCount);
	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	__m256 xMax = _mm256_load_ps(xValues);
	__m256 xMin = xMax;
	__m256 yMax = _mm256_load_ps(yValues);
	__m256 yMin = yMax;
	__m256 zMax = _mm256_load_ps(zValues);
	__m256 zMin = zMax;

	for(size_t blockI = 1; blockI < (vertexCount + 7) / 8; blockI++) {

		__m256 xVal = _mm256_load_ps(xValues + blockI * 8);
		__m256 yVal = _mm256_load_ps(yValues + blockI * 8);
		__m256 zVal = _mm256_load_ps(zValues + blockI * 8);

		xMax = _mm256_max_ps(xMax, xVal);
		yMax = _mm256_max_ps(yMax, yVal);
		zMax = _mm256_max_ps(zMax, zVal);

		xMin = _mm256_min_ps(xMin, xVal);
		yMin = _mm256_min_ps(yMin, yVal);
		zMin = _mm256_min_ps(zMin, zVal);
	}

	return toBounds(xMin, xMax, yMin, yMax, zMin, zMax);
}

BoundingBox TriangleMesh::getBounds(const Mat3f& referenceFrame) const {
	size_t vertexCount = this->vertexCount;

	size_t offset = getOffset(vertexCount);
	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	float mxx = referenceFrame(0, 0);
	float mxy = referenceFrame(0, 1);
	float mxz = referenceFrame(0, 2);
	float myx = referenceFrame(1, 0);
	float myy = referenceFrame(1, 1);
	float myz = referenceFrame(1, 2);
	float mzx = referenceFrame(2, 0);
	float mzy = referenceFrame(2, 1);
	float mzz = referenceFrame(2, 2);

	__m256 xVal = _mm256_load_ps(xValues);
	__m256 yVal = _mm256_load_ps(yValues);
	__m256 zVal = _mm256_load_ps(zValues);

	__m256 xMin = _mm256_fmadd_ps(_mm256_set1_ps(mxz), zVal, _mm256_fmadd_ps(_mm256_set1_ps(mxy), yVal, _mm256_mul_ps(_mm256_set1_ps(mxx), xVal)));
	__m256 yMin = _mm256_fmadd_ps(_mm256_set1_ps(myz), zVal, _mm256_fmadd_ps(_mm256_set1_ps(myy), yVal, _mm256_mul_ps(_mm256_set1_ps(myx), xVal)));
	__m256 zMin = _mm256_fmadd_ps(_mm256_set1_ps(mzz), zVal, _mm256_fmadd_ps(_mm256_set1_ps(mzy), yVal, _mm256_mul_ps(_mm256_set1_ps(mzx), xVal)));
	
	__m256 xMax = xMin;
	__m256 yMax = yMin;
	__m256 zMax = zMin;

	for(size_t blockI = 1; blockI < (vertexCount + 7) / 8; blockI++) {
		__m256 xVal = _mm256_load_ps(xValues + blockI * 8);
		__m256 yVal = _mm256_load_ps(yValues + blockI * 8);
		__m256 zVal = _mm256_load_ps(zValues + blockI * 8);

		__m256 dotX = _mm256_fmadd_ps(_mm256_set1_ps(mxz), zVal, _mm256_fmadd_ps(_mm256_set1_ps(mxy), yVal, _mm256_mul_ps(_mm256_set1_ps(mxx), xVal)));
		xMin = _mm256_min_ps(xMin, dotX);
		xMax = _mm256_max_ps(xMax, dotX);
		__m256 dotY = _mm256_fmadd_ps(_mm256_set1_ps(myz), zVal, _mm256_fmadd_ps(_mm256_set1_ps(myy), yVal, _mm256_mul_ps(_mm256_set1_ps(myx), xVal)));
		yMin = _mm256_min_ps(yMin, dotY);
		yMax = _mm256_max_ps(yMax, dotY);
		__m256 dotZ = _mm256_fmadd_ps(_mm256_set1_ps(mzz), zVal, _mm256_fmadd_ps(_mm256_set1_ps(mzy), yVal, _mm256_mul_ps(_mm256_set1_ps(mzx), xVal)));
		zMin = _mm256_min_ps(zMin, dotZ);
		zMax = _mm256_max_ps(zMax, dotZ);
	}

	return toBounds(xMin, xMax, yMin, yMax, zMin, zMax);
}


#elif _M_IX86_FP <= 2

#include <immintrin.h>
#ifdef _MSC_VER
#define GET_SSE_ELEM(reg, index) reg.m128_f32[index]
#else
#define GET_SSE_ELEM(reg, index) reg[index]
#endif

#define SWAP_2x2 0b01001110
#define SWAP_1x1 0b10110001

#ifdef _MSC_VER
inline static uint32_t countZeros(uint32_t mask) {
	unsigned long ret = 0;
	_BitScanForward(&ret, mask);
	return static_cast<int>(ret);
}
#else
inline static uint32_t countZeros(uint32_t mask) {
	return __builtin_ctz(mask);
}
#endif

inline __m128i _mm_blendv_epi32(__m128i a, __m128i b,__m128 mask) {
	return _mm_castps_si128(
		_mm_blendv_ps(
			_mm_castsi128_ps(a),
			_mm_castsi128_ps(b),
			mask
		)
	);
}

inline uint32_t mm_extract_epi32_var_indx(__m128i vec, int i) {
	__m128i indx = _mm_cvtsi32_si128(i);
	__m128i val = _mm_castps_si128(_mm_permutevar_ps(_mm_castsi128_ps(vec), indx));
	return         _mm_cvtsi128_si32(val);
}

BoundingBox toBounds(__m128 xMin, __m128 xMax, __m128 yMin, __m128 yMax, __m128 zMin, __m128 zMax) {

	for (int i = 0; i < 3; i++) {
		__m128 xShuf = _mm_shuffle_ps(xMax, xMax, 0x93);
		xMax = _mm_max_ps(xMax, xShuf);
	}

	for (int i = 0; i < 3; i++) {
		__m128 yShuf = _mm_shuffle_ps(yMax, yMax, 0x93);
		yMax = _mm_max_ps(yMax, yShuf);
	}

	for (int i = 0; i < 3; i++) {
		__m128 zShuf = _mm_shuffle_ps(zMax, zMax, 0x93);
		xMax = _mm_max_ps(zMax, zShuf);
	}

	for (int i = 0; i < 3; i++) {
		__m128 xShuf = _mm_shuffle_ps(xMin, xMin, 0x93);
		xMin = _mm_min_ps(xMax, xShuf);
	}
	
	for (int i = 0; i < 3; i++) {
		__m128 yShuf = _mm_shuffle_ps(yMin, yMin, 0x93);
		yMin = _mm_min_ps(yMin, yShuf);
	}

	for (int i = 0; i < 3; i++) {
		__m128 zShuf = _mm_shuffle_ps(zMin, zMin, 0x93);
		zMin = _mm_min_ps(zMin, zShuf);
	}

	return BoundingBox{ GET_SSE_ELEM(xMin,0), GET_SSE_ELEM(yMin, 0), GET_SSE_ELEM(zMin, 0), GET_SSE_ELEM(xMax, 0), GET_SSE_ELEM(yMax, 0), GET_SSE_ELEM(zMax, 0) };
}

BoundingBox TriangleMesh::getBounds() const {
	size_t vertexCount = this->vertexCount;

	size_t offset = getOffset(vertexCount);
	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	__m128 xMax = _mm_load_ps(xValues);
	__m128 xMin = xMax;
	__m128 yMax = _mm_load_ps(yValues);
	__m128 yMin = yMax;
	__m128 zMax = _mm_load_ps(zValues);
	__m128 zMin = zMax;

	for (size_t blockI = 1; blockI < (vertexCount + 3) / 4; blockI++) {
		__m128 xVal = _mm_load_ps(xValues + blockI * 4);
		__m128 yVal = _mm_load_ps(yValues + blockI * 4);
		__m128 zVal = _mm_load_ps(zValues + blockI * 4);

		xMax = _mm_max_ps(xMax, xVal);
		yMax = _mm_max_ps(yMax, yVal);
		zMax = _mm_max_ps(zMax, zVal);

		xMin = _mm_min_ps(xMin, xVal);
		yMin = _mm_min_ps(yMin, yVal);
		zMin = _mm_min_ps(zMin, zVal);
	}
	return toBounds(xMin, xMax, yMin, yMax, zMin, zMax);
}

BoundingBox TriangleMesh::getBounds(const Mat3f& referenceFrame) const {
	size_t vertexCount = this->vertexCount;

	size_t offset = getOffset(vertexCount);
	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	float mxx = referenceFrame(0, 0);
	float mxy = referenceFrame(0, 1);
	float mxz = referenceFrame(0, 2);
	float myx = referenceFrame(1, 0);
	float myy = referenceFrame(1, 1);
	float myz = referenceFrame(1, 2);
	float mzx = referenceFrame(2, 0);
	float mzy = referenceFrame(2, 1);
	float mzz = referenceFrame(2, 2);

	__m128 xVal = _mm_load_ps(xValues);
	__m128 yVal = _mm_load_ps(yValues);
	__m128 zVal = _mm_load_ps(zValues);

	__m128 xMin = _mm_fmadd_ps(_mm_set1_ps(mxz), zVal, _mm_fmadd_ps(_mm_set1_ps(mxy), yVal, _mm_mul_ps(_mm_set1_ps(mxx), xVal)));
	__m128 yMin = _mm_fmadd_ps(_mm_set1_ps(myz), zVal, _mm_fmadd_ps(_mm_set1_ps(myy), yVal, _mm_mul_ps(_mm_set1_ps(myx), xVal)));
	__m128 zMin = _mm_fmadd_ps(_mm_set1_ps(mzz), zVal, _mm_fmadd_ps(_mm_set1_ps(mzy), yVal, _mm_mul_ps(_mm_set1_ps(mzx), xVal)));

	__m128 xMax = xMin;
	__m128 yMax = yMin;
	__m128 zMax = zMin;

	for (size_t blockI = 1; blockI < (vertexCount + 3) / 4; blockI++) {
		__m128 xVal = _mm_load_ps(xValues + blockI * 4);
		__m128 yVal = _mm_load_ps(yValues + blockI * 4);
		__m128 zVal = _mm_load_ps(zValues + blockI * 4);

		__m128 dotX = _mm_fmadd_ps(_mm_set1_ps(mxz), zVal, _mm_fmadd_ps(_mm_set1_ps(mxy), yVal, _mm_mul_ps(_mm_set1_ps(mxx), xVal)));
		xMin = _mm_min_ps(xMin, dotX);
		xMax = _mm_max_ps(xMax, dotX);
		__m128 dotY = _mm_fmadd_ps(_mm_set1_ps(myz), zVal, _mm_fmadd_ps(_mm_set1_ps(myy), yVal, _mm_mul_ps(_mm_set1_ps(myx), xVal)));
		yMin = _mm_min_ps(yMin, dotY);
		yMax = _mm_max_ps(yMax, dotY);
		__m128 dotZ = _mm_fmadd_ps(_mm_set1_ps(mzz), zVal, _mm_fmadd_ps(_mm_set1_ps(mzy), yVal, _mm_mul_ps(_mm_set1_ps(mzx), xVal)));
		zMin = _mm_min_ps(zMin, dotZ);
		zMax = _mm_max_ps(zMax, dotZ);
	}

	return toBounds(xMin, xMax, yMin, yMax, zMin, zMax);
}

int TriangleMesh::furthestIndexInDirection(const Vec3f& direction) const {
	size_t vertexCount = this->vertexCount;

	__m128 dx = _mm_set1_ps(direction.x);
	__m128 dy = _mm_set1_ps(direction.y);
	__m128 dz = _mm_set1_ps(direction.z);

	size_t offset = getOffset(vertexCount);
	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	__m128 bestDot = _mm_fmadd_ps(dz, _mm_load_ps(zValues), _mm_fmadd_ps(dy, _mm_load_ps(yValues), _mm_mul_ps(dx, _mm_load_ps(xValues))));

	__m128i bestIndices = _mm_set1_epi32(0);

	for (size_t blockI = 1; blockI < (vertexCount + 3) / 4; blockI++) {
		__m128i indices = _mm_set1_epi32(int(blockI));

		__m128 dot = _mm_fmadd_ps(dz, _mm_load_ps(zValues + blockI * 4), _mm_fmadd_ps(dy, _mm_load_ps(yValues + blockI * 4), _mm_mul_ps(dx, _mm_load_ps(xValues + blockI * 4))));

		__m128 whichAreMax = _mm_cmpgt_ps(dot, bestDot); 

		bestDot = _mm_blendv_ps(bestDot, dot, whichAreMax);
		bestIndices = _mm_blendv_epi32(bestIndices, indices, whichAreMax);
	}

	// find max of our 8 left candidates
	__m128 swap4x4 = _mm_shuffle_ps(bestDot, bestDot, 0x1);
	__m128 bestDotInternalMax = _mm_max_ps(bestDot, swap4x4);
	__m128 swap2x2 = _mm_shuffle_ps(bestDotInternalMax, bestDotInternalMax, SWAP_2x2);
	bestDotInternalMax = _mm_max_ps(bestDotInternalMax, swap2x2);
	__m128 swap1x1 = _mm_shuffle_ps(bestDotInternalMax, bestDotInternalMax, SWAP_1x1);
	bestDotInternalMax = _mm_max_ps(bestDotInternalMax, swap1x1);

	__m128 compare = _mm_cmp_ps(bestDotInternalMax, bestDot, _CMP_EQ_UQ);
	uint32_t mask = _mm_movemask_ps(compare);

	assert(mask != 0);

	uint32_t index = countZeros(mask);
	uint32_t block = mm_extract_epi32_var_indx(bestIndices, index);
	return block * 8 + index;
}

Vec3f TriangleMesh::furthestInDirection(const Vec3f& direction) const {
	size_t vertexCount = this->vertexCount;

	__m128 dx = _mm_set1_ps(direction.x);
	__m128 dy = _mm_set1_ps(direction.y);
	__m128 dz = _mm_set1_ps(direction.z);

	size_t offset = getOffset(vertexCount);
	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	__m128 bestX = _mm_load_ps(xValues);
	__m128 bestY = _mm_load_ps(yValues);
	__m128 bestZ = _mm_load_ps(zValues);

	__m128 bestDot = _mm_fmadd_ps(dz, bestZ, _mm_fmadd_ps(dy, bestY, _mm_mul_ps(dx, bestX)));

	for (size_t blockI = 1; blockI < (vertexCount + 3) / 4; blockI++) {
		__m128i indices = _mm_set1_epi32(int(blockI));

		__m128 xVal = _mm_load_ps(xValues + blockI * 4);
		__m128 yVal = _mm_load_ps(yValues + blockI * 4);
		__m128 zVal = _mm_load_ps(zValues + blockI * 4);

		__m128 dot = _mm_fmadd_ps(dz, zVal, _mm_fmadd_ps(dy, yVal, _mm_mul_ps(dx, xVal)));

		__m128 whichAreMax = _mm_cmp_ps(dot, bestDot, _CMP_GT_OQ); // Greater than, false if dot == NaN
		bestDot = _mm_blendv_ps(bestDot, dot, whichAreMax);
		bestX = _mm_blendv_ps(bestX, xVal, whichAreMax);
		bestY = _mm_blendv_ps(bestY, yVal, whichAreMax);
		bestZ = _mm_blendv_ps(bestZ, zVal, whichAreMax);
	}

	// now we find the max of the remaining 8 elements
	__m128 swap4x4 = _mm_shuffle_ps(bestDot, bestDot, 1);
	__m128 bestDotInternalMax = _mm_max_ps(bestDot, swap4x4);
	__m128 swap2x2 = _mm_shuffle_ps(bestDotInternalMax, bestDotInternalMax, SWAP_2x2);
	bestDotInternalMax = _mm_max_ps(bestDotInternalMax, swap2x2);
	__m128 swap1x1 = _mm_shuffle_ps(bestDotInternalMax, bestDotInternalMax, SWAP_1x1);
	bestDotInternalMax = _mm_max_ps(bestDotInternalMax, swap1x1);

	__m128 compare = _mm_cmp_ps(bestDotInternalMax, bestDot, _CMP_EQ_UQ);
	uint32_t mask = _mm_movemask_ps(compare);

	assert(mask != 0);

	uint32_t index = countZeros(mask);

	// a bug occurs here, when mask == 0 the resulting index is undefined

	return Vec3f(GET_SSE_ELEM(bestX, index), GET_SSE_ELEM(bestY, index), GET_SSE_ELEM(bestZ, index));
}

#else
int TriangleMesh::furthestIndexInDirection(const Vec3f& direction) const {
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

Vec3f TriangleMesh::furthestInDirection(const Vec3f& direction) const {
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


BoundingBox TriangleMesh::getBounds() const {
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

BoundingBox TriangleMesh::getBounds(const Mat3f& referenceFrame) const {
	Mat3f transp = referenceFrame.transpose();
	double xmax = (referenceFrame * this->furthestInDirection(transp * Vec3f(1, 0, 0))).x;
	double xmin = (referenceFrame * this->furthestInDirection(transp * Vec3f(-1, 0, 0))).x;
	double ymax = (referenceFrame * this->furthestInDirection(transp * Vec3f(0, 1, 0))).y;
	double ymin = (referenceFrame * this->furthestInDirection(transp * Vec3f(0, -1, 0))).y;
	double zmax = (referenceFrame * this->furthestInDirection(transp * Vec3f(0, 0, 1))).z;
	double zmin = (referenceFrame * this->furthestInDirection(transp * Vec3f(0, 0, -1))).z;

	return BoundingBox(xmin, ymin, zmin, xmax, ymax, zmax);
}

#endif
#pragma endregion
