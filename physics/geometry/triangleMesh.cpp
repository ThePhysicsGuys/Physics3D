#include "triangleMesh.h"

#include "../misc/validityHelper.h"

#include <stddef.h>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <set>
#include <math.h>


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

static UniqueAlignedPointer<float> createAndFillParallelVecBuf(size_t size, const Vec3f* vectors) {
	UniqueAlignedPointer<float> buf = createParallelVecBuf(size);

	size_t offset = getOffset(size);

	float* xValues = buf;
	float* yValues = buf + offset;
	float* zValues = buf + 2 * offset;

	for(size_t i = 0; i < size; i++) {
		xValues[i] = vectors[i].x;
		yValues[i] = vectors[i].y;
		zValues[i] = vectors[i].z;
	}
	fixFinalBlock(buf.get(), size);

	return buf;
}

static UniqueAlignedPointer<int> createAndFillParallelTriangleBuf(size_t size, const Triangle* triangles) {
	UniqueAlignedPointer<int> buf = createParallelTriangleBuf(size);

	size_t offset = getOffset(size);

	int* aValues = buf;
	int* bValues = buf + offset;
	int* cValues = buf + 2 * offset;

	for(size_t i = 0; i < size; i++) {
		aValues[i] = triangles[i].firstIndex;
		bValues[i] = triangles[i].secondIndex;
		cValues[i] = triangles[i].thirdIndex;
	}
	fixFinalBlock(buf.get(), size);

	return buf;
}

static void setInBuf(float* buf, size_t size, size_t index, const Vec3f& value) {
	size_t offset = getOffset(size);

	float* xValues = buf;
	float* yValues = buf + offset;
	float* zValues = buf + 2 * offset;

	xValues[index] = value.x;
	yValues[index] = value.y;
	zValues[index] = value.z;
}



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


TriangleMesh::TriangleMesh(UniqueAlignedPointer<float>&& vertices, UniqueAlignedPointer<int>&& triangles, int vertexCount, int triangleCount) :
	vertices(std::move(vertices)), triangles(std::move(triangles)), vertexCount(vertexCount), triangleCount(triangleCount) {}

TriangleMesh::TriangleMesh(const Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount) :
	vertices(createAndFillParallelVecBuf(vertexCount, vertices)),
	triangles(createAndFillParallelTriangleBuf(triangleCount, triangles)),
	vertexCount(vertexCount),
	triangleCount(triangleCount) {
	assert(isValid(*this));
}

TriangleMesh::~TriangleMesh() {}

TriangleMesh::TriangleMesh(TriangleMesh&& mesh) noexcept :
	vertices(std::move(mesh.vertices)),
	triangles(std::move(mesh.triangles)),
	vertexCount(mesh.vertexCount),
	triangleCount(mesh.triangleCount) {

}

TriangleMesh::TriangleMesh(const TriangleMesh& mesh) :
	vertices(copy(mesh.vertices, mesh.vertexCount)),
	triangles(copy(mesh.triangles, mesh.triangleCount)),
	vertexCount(mesh.vertexCount),
	triangleCount(mesh.triangleCount) {

}

TriangleMesh& TriangleMesh::operator=(TriangleMesh&& mesh) noexcept {
	this->vertices = std::move(mesh.vertices);
	this->triangles = std::move(mesh.triangles);
	this->vertexCount = mesh.vertexCount;
	this->triangleCount = mesh.triangleCount;

	return *this;
}

TriangleMesh& TriangleMesh::operator=(const TriangleMesh& mesh) {
	this->vertices = copy(mesh.vertices, mesh.vertexCount);
	this->triangles = copy(mesh.triangles, mesh.triangleCount);
	this->vertexCount = mesh.vertexCount;
	this->triangleCount = mesh.triangleCount;

	return *this;
}

Vec3f TriangleMesh::getVertex(int index) const {
	return Vec3f(this->vertices[index], this->vertices[index + getOffset(vertexCount)], this->vertices[index + 2 * getOffset(vertexCount)]);
}

Triangle TriangleMesh::getTriangle(int index) const {
	size_t offset = getOffset(triangleCount);
	return Triangle{triangles[index], triangles[index + offset], triangles[index + 2 * offset]};
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
	UniqueAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for(int i = 0; i < this->vertexCount; i++) {
		setInBuf(newBuf, vertexCount, i, this->getVertex(i) + offset);
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return TriangleMesh(std::move(newBuf), copy(triangles, triangleCount), vertexCount, triangleCount);
}

TriangleMesh TriangleMesh::rotated(Rotationf rotation) const {
	UniqueAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for(int i = 0; i < this->vertexCount; i++) {
		setInBuf(newBuf, vertexCount, i, rotation * this->getVertex(i));
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return TriangleMesh(std::move(newBuf), copy(triangles, triangleCount), vertexCount, triangleCount);
}

TriangleMesh TriangleMesh::localToGlobal(CFramef frame) const {
	UniqueAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for(int i = 0; i < this->vertexCount; i++) {
		setInBuf(newBuf, vertexCount, i, frame.localToGlobal(this->getVertex(i)));
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return TriangleMesh(std::move(newBuf), copy(triangles, triangleCount), vertexCount, triangleCount);
}

TriangleMesh TriangleMesh::globalToLocal(CFramef frame) const {
	UniqueAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for(int i = 0; i < this->vertexCount; i++) {
		setInBuf(newBuf, vertexCount, i, frame.globalToLocal(this->getVertex(i)));
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return TriangleMesh(std::move(newBuf), copy(triangles, triangleCount), vertexCount, triangleCount);
}
TriangleMesh TriangleMesh::scaled(float scaleX, float scaleY, float scaleZ) const {
	UniqueAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for(int i = 0; i < this->vertexCount; i++) {
		Vec3f v = this->getVertex(i);
		setInBuf(newBuf, vertexCount, i, Vec3f(scaleX * v.x, scaleY * v.y, scaleZ * v.z));
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return TriangleMesh(std::move(newBuf), copy(triangles, triangleCount), vertexCount, triangleCount);
}
TriangleMesh TriangleMesh::scaled(DiagonalMat3f scale) const { 
	return scaled(scale[0], scale[1], scale[2]); 
}

TriangleMesh TriangleMesh::translatedAndScaled(Vec3f translation, DiagonalMat3f scale) const {
	UniqueAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for(int i = 0; i < this->vertexCount; i++) {
		Vec3f cur = this->getVertex(i);
		setInBuf(newBuf, vertexCount, i, scale * (cur + translation));
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return TriangleMesh(std::move(newBuf), copy(triangles, triangleCount), vertexCount, triangleCount);
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


#ifdef __AVX__
#include <immintrin.h>
#if defined(_MSC_VER) && _MSC_VER == 1922
inline static uint32_t __builtin_ctz(uint32_t x) {
	unsigned long ret;
	_BitScanForward(&ret, x);
	return ( int) ret;
}
#endif

#ifdef _MSC_VER
#define GET_AVX_ELEM(reg, index) reg.m256_f32[index]
#else
#define GET_AVX_ELEM(reg, index) reg[index]
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

	__m256 xTxd = _mm256_mul_ps(dx, _mm256_load_ps(xValues));
	__m256 yTyd = _mm256_mul_ps(dy, _mm256_load_ps(yValues));
	__m256 zTzd = _mm256_mul_ps(dz, _mm256_load_ps(zValues));

	__m256 bestDot = _mm256_add_ps(_mm256_add_ps(xTxd, yTyd), zTzd);
	__m256i bestIndices = _mm256_set1_epi32(0);

	for(size_t blockI = 1; blockI < (vertexCount + 7) / 8; blockI++) {
		__m256i indices = _mm256_set1_epi32(int(blockI));

		__m256 xTxd = _mm256_mul_ps(dx, _mm256_load_ps(xValues + blockI * 8));
		__m256 yTyd = _mm256_mul_ps(dy, _mm256_load_ps(yValues + blockI * 8));
		__m256 zTzd = _mm256_mul_ps(dz, _mm256_load_ps(zValues + blockI * 8));
		__m256 dot = _mm256_add_ps(_mm256_add_ps(xTxd, yTyd), zTzd);

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

	uint32_t index = __builtin_ctz(mask);
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

	__m256 xTxd = _mm256_mul_ps(dx, bestX);
	__m256 yTyd = _mm256_mul_ps(dy, bestY);
	__m256 zTzd = _mm256_mul_ps(dz, bestZ);

	__m256 bestDot = _mm256_add_ps(_mm256_add_ps(xTxd, yTyd), zTzd);

	for(size_t blockI = 1; blockI < (vertexCount + 7) / 8; blockI++) {
		__m256i indices = _mm256_set1_epi32(int(blockI));

		__m256 xVal = _mm256_load_ps(xValues + blockI * 8);
		__m256 yVal = _mm256_load_ps(yValues + blockI * 8);
		__m256 zVal = _mm256_load_ps(zValues + blockI * 8);

		__m256 xTxd = _mm256_mul_ps(dx, xVal);
		__m256 yTyd = _mm256_mul_ps(dy, yVal);
		__m256 zTzd = _mm256_mul_ps(dz, zVal);
		__m256 dot = _mm256_add_ps(_mm256_add_ps(xTxd, yTyd), zTzd);

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

	uint32_t index = __builtin_ctz(mask);

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
		__m256i indices = _mm256_set1_epi32(int(blockI));

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

	Vec3f xDir = referenceFrame.getRow(0);
	Vec3f yDir = referenceFrame.getRow(1);
	Vec3f zDir = referenceFrame.getRow(2);

	__m256 xVal = _mm256_load_ps(xValues);
	__m256 yVal = _mm256_load_ps(yValues);
	__m256 zVal = _mm256_load_ps(zValues);

	__m256 xTx = _mm256_mul_ps(_mm256_set1_ps(xDir.x), xVal);
	__m256 xTy = _mm256_mul_ps(_mm256_set1_ps(xDir.y), yVal);
	__m256 xTz = _mm256_mul_ps(_mm256_set1_ps(xDir.z), zVal);
	__m256 xMin = _mm256_add_ps(_mm256_add_ps(xTx, xTy), xTz);
	__m256 xMax = xMin;

	__m256 yTx = _mm256_mul_ps(_mm256_set1_ps(yDir.x), xVal);
	__m256 yTy = _mm256_mul_ps(_mm256_set1_ps(yDir.y), yVal);
	__m256 yTz = _mm256_mul_ps(_mm256_set1_ps(yDir.z), zVal);
	__m256 yMin = _mm256_add_ps(_mm256_add_ps(yTx, yTy), yTz);
	__m256 yMax = yMin;

	__m256 zTx = _mm256_mul_ps(_mm256_set1_ps(zDir.x), xVal);
	__m256 zTy = _mm256_mul_ps(_mm256_set1_ps(zDir.y), yVal);
	__m256 zTz = _mm256_mul_ps(_mm256_set1_ps(zDir.z), zVal);
	__m256 zMin = _mm256_add_ps(_mm256_add_ps(zTx, zTy), zTz);
	__m256 zMax = zMin;

	for(size_t blockI = 1; blockI < (vertexCount + 7) / 8; blockI++) {
		__m256 xVal = _mm256_load_ps(xValues + blockI * 8);
		__m256 yVal = _mm256_load_ps(yValues + blockI * 8);
		__m256 zVal = _mm256_load_ps(zValues + blockI * 8);

		__m256 xTx = _mm256_mul_ps(_mm256_set1_ps(xDir.x), xVal);
		__m256 xTy = _mm256_mul_ps(_mm256_set1_ps(xDir.y), yVal);
		__m256 xTz = _mm256_mul_ps(_mm256_set1_ps(xDir.z), zVal);
		__m256 dotX = _mm256_add_ps(_mm256_add_ps(xTx, xTy), xTz);

		__m256 yTx = _mm256_mul_ps(_mm256_set1_ps(yDir.x), xVal);
		__m256 yTy = _mm256_mul_ps(_mm256_set1_ps(yDir.y), yVal);
		__m256 yTz = _mm256_mul_ps(_mm256_set1_ps(yDir.z), zVal);
		__m256 dotY = _mm256_add_ps(_mm256_add_ps(yTx, yTy), yTz);

		__m256 zTx = _mm256_mul_ps(_mm256_set1_ps(zDir.x), xVal);
		__m256 zTy = _mm256_mul_ps(_mm256_set1_ps(zDir.y), yVal);
		__m256 zTz = _mm256_mul_ps(_mm256_set1_ps(zDir.z), zVal);
		__m256 dotZ = _mm256_add_ps(_mm256_add_ps(zTx, zTy), zTz);

		xMin = _mm256_min_ps(xMin, dotX);
		xMax = _mm256_max_ps(xMax, dotX);
		yMin = _mm256_min_ps(yMin, dotY);
		yMax = _mm256_max_ps(yMax, dotY);
		zMin = _mm256_min_ps(zMin, dotZ);
		zMax = _mm256_max_ps(zMax, dotZ);
	}

	return toBounds(xMin, xMax, yMin, yMax, zMin, zMax);
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
