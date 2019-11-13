#include "polyhedron.h"

#include <stdlib.h>
#include <cstring>
#include <vector>
#include <set>
#include <math.h>

#include "../math/linalg/vec.h"
#include "../math/linalg/trigonometry.h"
#include "../math/utils.h"
#include "../math/mathUtil.h"
#include "../../util/Log.h"
#include "../debug.h"
#include "../physicsProfiler.h"
#include "normalizedPolyhedron.h"
#include "shape.h"

#include "polyhedronInternals.h"

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

Triangle Triangle::operator~() const {
	return Triangle { firstIndex, thirdIndex, secondIndex };
}

bool Triangle::operator==(const Triangle & other) const {
	return firstIndex == other.firstIndex && secondIndex == other.secondIndex && thirdIndex == other.thirdIndex || 
		firstIndex == other.secondIndex && secondIndex == other.thirdIndex && thirdIndex == other.firstIndex ||
		firstIndex == other.thirdIndex && secondIndex == other.firstIndex && thirdIndex == other.secondIndex;
}

Triangle Triangle::rightShift() const { 
	return Triangle { thirdIndex, firstIndex, secondIndex };
}

Triangle Triangle::leftShift() const {
	return Triangle { secondIndex, thirdIndex, firstIndex };
}

inline Polyhedron::Polyhedron(UniqueAlignedPointer<float>&& vertices, UniqueAlignedPointer<int>&& triangles, int vertexCount, int triangleCount) :
	vertices(std::move(vertices)), triangles(std::move(triangles)), vertexCount(vertexCount), triangleCount(triangleCount) {
}

Polyhedron::~Polyhedron() {
	
}

Polyhedron::Polyhedron(const Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount) :
	vertices(createAndFillParallelVecBuf(vertexCount, vertices)),
	triangles(createAndFillParallelTriangleBuf(triangleCount, triangles)),
	vertexCount(vertexCount), 
	triangleCount(triangleCount) {}

Polyhedron::Polyhedron(const Polyhedron& poly) : 
	vertices(copy(poly.vertices, poly.vertexCount)), 
	triangles(copy(poly.triangles, poly.triangleCount)), 
	vertexCount(poly.vertexCount), 
	triangleCount(poly.triangleCount) {}


Polyhedron& Polyhedron::operator=(Polyhedron&& poly) noexcept {
	this->vertices = std::move(poly.vertices);
	this->triangles = std::move(poly.triangles);
	this->vertexCount = poly.vertexCount;
	this->triangleCount = poly.triangleCount;
	return *this;
}
Polyhedron& Polyhedron::operator=(const Polyhedron& poly) {
	this->vertices = copy(poly.vertices, poly.vertexCount);
	this->triangles = copy(poly.triangles, poly.triangleCount);
	this->vertexCount = poly.vertexCount;
	this->triangleCount = poly.triangleCount;
	return *this;
}

Vec3f Polyhedron::operator[](int index) const {
	return Vec3f(this->vertices[index], this->vertices[index + getOffset(vertexCount)], this->vertices[index + 2*getOffset(vertexCount)]);
}

Polyhedron Polyhedron::translated(Vec3f offset) const {
	UniqueAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for (int i = 0; i < this->vertexCount; i++) {
		setInBuf(newBuf, vertexCount, i, (*this)[i] + offset);
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return Polyhedron(std::move(newBuf), copy(triangles, triangleCount), vertexCount, triangleCount);
}

Polyhedron Polyhedron::rotated(RotMat3f rotation) const {
	UniqueAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for (int i = 0; i < this->vertexCount; i++) {
		setInBuf(newBuf, vertexCount, i, rotation * (*this)[i]);
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return Polyhedron(std::move(newBuf), copy(triangles, triangleCount), vertexCount, triangleCount);
}

Polyhedron Polyhedron::localToGlobal(CFramef frame) const {
	UniqueAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for (int i = 0; i < this->vertexCount; i++) {
		setInBuf(newBuf, vertexCount, i, frame.localToGlobal((*this)[i]));
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return Polyhedron(std::move(newBuf), copy(triangles, triangleCount), vertexCount, triangleCount);
}

Polyhedron Polyhedron::globalToLocal(CFramef frame) const {
	UniqueAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for (int i = 0; i < this->vertexCount; i++) {
		setInBuf(newBuf, vertexCount, i, frame.globalToLocal((*this)[i]));
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return Polyhedron(std::move(newBuf), copy(triangles, triangleCount), vertexCount, triangleCount);
}
Polyhedron Polyhedron::scaled(float scaleX, float scaleY, float scaleZ) const {
	UniqueAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for (int i = 0; i < this->vertexCount; i++) {
		Vec3f v = (*this)[i];
		setInBuf(newBuf, vertexCount, i, Vec3f(scaleX * v.x, scaleY * v.y, scaleZ * v.z));
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return Polyhedron(std::move(newBuf), copy(triangles, triangleCount), vertexCount, triangleCount);
}
Polyhedron Polyhedron::scaled(double scaleX, double scaleY, double scaleZ) const {
	return scaled(static_cast<float>(scaleX), static_cast<float>(scaleY), static_cast<float>(scaleZ));
}

Polyhedron Polyhedron::translatedAndScaled(Vec3f translation, DiagonalMat3f scale) const {
	UniqueAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for(int i = 0; i < this->vertexCount; i++) {
		Vec3f cur = (*this)[i];
		setInBuf(newBuf, vertexCount, i, scale * (cur + translation));
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return Polyhedron(std::move(newBuf), copy(triangles, triangleCount), vertexCount, triangleCount);
}

// for every edge, of every triangle, check that it coincides with exactly one other triangle, in reverse order
bool isComplete(ShapeTriangleIter iter, ShapeTriangleIter fin) {
	for (; iter != fin; ++iter) {
		Triangle a = *iter;

		ShapeTriangleIter iter2 = iter;
		++iter2;
		for (; iter2 != fin; ++iter2) {
			Triangle b = *iter2;

			if (a.sharesEdgeWith(b)) {  // correctly oriented
				goto endOfLoop;
			} else if (a.sharesEdgeWith(~b)) {	// wrongly oriented
				Log::warn("triangle (%d, %d, %d) and triangle (%d, %d, %d) are joined wrongly", a.firstIndex, a.secondIndex, a.thirdIndex, b.firstIndex, b.secondIndex, b.thirdIndex);
				return false;
			}
		}
		Log::warn("No triangle found that shares an edge with triangle(%d, %d, %d)", a.firstIndex, a.secondIndex, a.thirdIndex);
		return false;
		endOfLoop:;
	}
	return true;
}

bool Polyhedron::isValid() const {
	IteratorFactory<ShapeTriangleIter> f = iterTriangles();
	return isComplete(f.begin(), f.end()) && getVolume() >= 0;
}

Vec3f Polyhedron::getNormalVecOfTriangle(Triangle triangle) const {
	Vec3f v0 = (*this)[triangle.firstIndex];
	return ((*this)[triangle.secondIndex] - v0) % ((*this)[triangle.thirdIndex] - v0);
}


// TODO parallelize
void Polyhedron::computeNormals(Vec3f* buffer) const {
	for (Triangle triangle : iterTriangles()) {
		Vec3f v0 = (*this)[triangle.firstIndex];
		Vec3f v1 = (*this)[triangle.secondIndex];
		Vec3f v2 = (*this)[triangle.thirdIndex];

		Vec3f D1 = v1 - v0;
		Vec3f D2 = v2 - v0;

		Vec3f faceNormal = normalize(D1 % D2);

		buffer[triangle.firstIndex] += faceNormal;
		buffer[triangle.secondIndex] += faceNormal;
		buffer[triangle.thirdIndex] += faceNormal;
	}

	for (int i = 0; i < vertexCount; i++) {
		buffer[i] = normalize(buffer[i]);
	}
}

//TODO parallelize
bool Polyhedron::containsPoint(Vec3f point) const {
	Vec3f ray(1, 0, 0);

	bool isExiting = false;
	double bestD = static_cast<double>(INFINITY);

	for(const Triangle& tri : iterTriangles()) {
		RayIntersection<float> r = rayTriangleIntersection(point, ray, (*this)[tri.firstIndex], (*this)[tri.secondIndex], (*this)[tri.thirdIndex]);
		if(r.d >= 0 && r.lineIntersectsTriangle()) {
			if(r.d < bestD) {
				bestD = r.d;
				isExiting = (getNormalVecOfTriangle(tri) * ray >= 0);
			}
		}
	}

	return isExiting;
}



#ifdef __AVX__
#include <immintrin.h>
#ifdef _MSC_VER

#define SWAP_2x2 0b01001110
#define SWAP_1x1 0b10110001


inline uint32_t __builtin_ctz(uint32_t x) {
	unsigned long ret;
	_BitScanForward(&ret, x);
	return (int)ret;
}
#endif
inline __m256i _mm256_blendv_epi32(__m256i a, __m256i b, __m256 mask) {
	return _mm256_castps_si256(
		_mm256_blendv_ps(
			_mm256_castsi256_ps(a),
			_mm256_castsi256_ps(b),
			mask
		)
	);
}

inline uint32_t mm256_extract_epi32_var_indx(__m256i vec, int i)
{
	__m128i indx = _mm_cvtsi32_si128(i);
	__m256i val = _mm256_permutevar8x32_epi32(vec, _mm256_castsi128_si256(indx));
	return         _mm_cvtsi128_si32(_mm256_castsi256_si128(val));
}

int Polyhedron::furthestIndexInDirection(const Vec3f& direction) const {
	__m256 dx = _mm256_set1_ps(direction.x);
	__m256 dy = _mm256_set1_ps(direction.y);
	__m256 dz = _mm256_set1_ps(direction.z);

	size_t offset = getOffset(this->vertexCount);
	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	__m256 xTxd = _mm256_mul_ps(dx, _mm256_load_ps(xValues));
	__m256 yTyd = _mm256_mul_ps(dy, _mm256_load_ps(yValues));
	__m256 zTzd = _mm256_mul_ps(dz, _mm256_load_ps(zValues));

	__m256 bestDot = _mm256_add_ps(_mm256_add_ps(xTxd, yTyd), zTzd);
	__m256i bestIndices = _mm256_set1_epi32(0);

	for(int blockI = 1; blockI < (vertexCount+7)/8; blockI++) {
		__m256i indices = _mm256_set1_epi32(blockI);

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

	__m256 compare = _mm256_cmp_ps(bestDotInternalMax, bestDot, _CMP_EQ_OQ);
	uint32_t mask = _mm256_movemask_ps(compare);
	uint32_t index = __builtin_ctz(mask);
	uint32_t block = mm256_extract_epi32_var_indx(bestIndices, index);
	return block * 8 + index;
}

Vec3f Polyhedron::furthestInDirection(const Vec3f& direction) const {
	__m256 dx = _mm256_set1_ps(direction.x);
	__m256 dy = _mm256_set1_ps(direction.y);
	__m256 dz = _mm256_set1_ps(direction.z);

	size_t offset = getOffset(this->vertexCount);
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

	for (int blockI = 1; blockI < (vertexCount + 7) / 8; blockI++) {
		__m256i indices = _mm256_set1_epi32(blockI);

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

	__m256 compare = _mm256_cmp_ps(bestDotInternalMax, bestDot, _CMP_EQ_OQ);
	uint32_t mask = _mm256_movemask_ps(compare);
	uint32_t index = __builtin_ctz(mask);
	
	return Vec3f(bestX.m256_f32[index], bestY.m256_f32[index], bestZ.m256_f32[index]);
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

	return BoundingBox{zxzyMin.m256_f32[2], zxzyMin.m256_f32[6], zxzyMin.m256_f32[0], zxzyMax.m256_f32[2], zxzyMax.m256_f32[6], zxzyMax.m256_f32[0]};
}

BoundingBox Polyhedron::getBounds() const {
	size_t offset = getOffset(this->vertexCount);
	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	__m256 xMax = _mm256_load_ps(xValues);
	__m256 xMin = xMax;
	__m256 yMax = _mm256_load_ps(yValues);
	__m256 yMin = yMax;
	__m256 zMax = _mm256_load_ps(zValues);
	__m256 zMin = zMax;

	for(int blockI = 1; blockI < (vertexCount + 7) / 8; blockI++) {
		__m256i indices = _mm256_set1_epi32(blockI);

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

BoundingBox Polyhedron::getBounds(const Mat3f& referenceFrame) const {
	size_t offset = getOffset(this->vertexCount);
	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	Vec3 xDir = referenceFrame.getRow(0);
	Vec3 yDir = referenceFrame.getRow(1);
	Vec3 zDir = referenceFrame.getRow(2);
	
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

	for(int blockI = 1; blockI < (vertexCount + 7) / 8; blockI++) {
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
int Polyhedron::furthestIndexInDirection(const Vec3f& direction) const {
	float bestDot = (*this)[0] * direction;
	int bestVertexIndex = 0;
	for(int i = 1; i < vertexCount; i++) {
		float newD = (*this)[i] * direction;
		if(newD > bestDot) {
			bestDot = newD;
			bestVertexIndex = i;
		}
	}

	return bestVertexIndex;
}

Vec3f Polyhedron::furthestInDirection(const Vec3f& direction) const {
	float bestDot = (*this)[0] * direction;
	Vec3f bestVertex = (*this)[0];
	for (int i = 1; i < vertexCount; i++) {
		float newD = (*this)[i] * direction;
		if (newD > bestDot) {
			bestDot = newD;
			bestVertex = (*this)[i];
		}
	}

	return bestVertex;
}


BoundingBox Polyhedron::getBounds() const {
	double xmin = (*this)[0].x, xmax = (*this)[0].x;
	double ymin = (*this)[0].y, ymax = (*this)[0].y;
	double zmin = (*this)[0].z, zmax = (*this)[0].z;

	for(int i = 1; i < vertexCount; i++) {
		const Vec3f current = (*this)[i];

		if(current.x < xmin) xmin = current.x;
		if(current.x > xmax) xmax = current.x;
		if(current.y < ymin) ymin = current.y;
		if(current.y > ymax) ymax = current.y;
		if(current.z < zmin) zmin = current.z;
		if(current.z > zmax) zmax = current.z;
	}

	return BoundingBox{xmin, ymin, zmin, xmax, ymax, zmax};
}

BoundingBox Polyhedron::getBounds(const Mat3f& referenceFrame) const {
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

double Polyhedron::getVolume() const {
	double total = 0;
	for (Triangle triangle : iterTriangles()) {
		Vec3f v0 = (*this)[triangle.firstIndex];
		Vec3f v1 = (*this)[triangle.secondIndex];
		Vec3f v2 = (*this)[triangle.thirdIndex];

		Vec3 D1 = v1 - v0; 
		Vec3 D2 = v2 - v0;
		
		double Tf = (D1.x * D2.y - D1.y * D2.x);

		total += Tf * ((D1.z + D2.z) / 6 + v0.z / 2);
	}

	return total;
}

Vec3 Polyhedron::getCenterOfMass() const {
	Vec3 total(0,0,0);
	for (Triangle triangle : iterTriangles()) {
		Vec3f v0 = (*this)[triangle.firstIndex];
		Vec3f v1 = (*this)[triangle.secondIndex];
		Vec3f v2 = (*this)[triangle.thirdIndex];

		Vec3f D1 = v1 - v0;
		Vec3f D2 = v2 - v0;

		Vec3f dFactor = D1 % D2;
		Vec3f vFactor = elementWiseSquare(v0) + elementWiseSquare(v1) + elementWiseSquare(v2) + elementWiseMul(v0, v1) + elementWiseMul(v1, v2) + elementWiseMul(v2, v0);

		total += Vec3(elementWiseMul(dFactor, vFactor));
	}
	
	return total / (24 * getVolume());
}

CircumscribingSphere Polyhedron::getCircumscribingSphere() const {
	BoundingBox bounds = getBounds();
	Vec3 center = Vec3(bounds.xmax + bounds.xmin, bounds.ymax + bounds.ymin, bounds.zmax + bounds.zmin) / 2.0;
	double radius = getMaxRadius(center);
	return CircumscribingSphere{center, radius};
}

double Polyhedron::getMaxRadiusSq() const {
	double bestDistSq = 0;
	for(Vec3f vertex : iterVertices()) {
		double distSq = lengthSquared(vertex);
		if(distSq > bestDistSq) {
			bestDistSq = distSq;
		}
	}
	return bestDistSq;
}

double Polyhedron::getMaxRadiusSq(Vec3f reference) const {
	double bestDistSq = 0;
	for (Vec3f vertex : iterVertices()) {
		double distSq = lengthSquared(vertex-reference);
		if (distSq > bestDistSq) {
			bestDistSq = distSq;
		}
	}
	return bestDistSq;
}

double Polyhedron::getMaxRadius() const {
	return sqrt(getMaxRadiusSq());
}

double Polyhedron::getMaxRadius(Vec3f reference) const {
	return sqrt(getMaxRadiusSq(reference));
}


double Polyhedron::getScaledMaxRadiusSq(DiagonalMat3 scale) const {
	double bestDistSq = 0;
	for(Vec3f vertex : iterVertices()) {
		double distSq = lengthSquared(scale * Vec3(vertex));
		if(distSq > bestDistSq) {
			bestDistSq = distSq;
		}
	}
	return bestDistSq;
}
double Polyhedron::getScaledMaxRadius(DiagonalMat3 scale) const {
	return sqrt(getScaledMaxRadiusSq(scale));
}

/*
	The total inertial matrix is given by the integral over the volume of the shape of the following matrix:
	[[
	[y^2+z^2,    xy,    xz],
	[xy,    x^2+z^2,    yz],
	[xz,    yz,    x^2+y^2]
	]]

	This has been reworked to a surface integral resulting in the given formulae

	This function returns an intermediary step which can easily be converted to scaled versions of the inertial matrix
*/
ScalableInertialMatrix Polyhedron::getScalableInertia(const CFrame& reference) const {
	Vec3 totalDiagElementParts(0, 0, 0);
	Vec3 totalOffDiag(0, 0, 0);
	for (Triangle triangle : iterTriangles()) {
		Vec3 v0 = reference.globalToLocal(Vec3((*this)[triangle.firstIndex]));
		Vec3 v1 = reference.globalToLocal(Vec3((*this)[triangle.secondIndex]));
		Vec3 v2 = reference.globalToLocal(Vec3((*this)[triangle.thirdIndex]));

		Vec3 D1 = v1 - v0; // scales x:sx y:sy z:sz
		Vec3 D2 = v2 - v0; // scales x:sx y:sy z:sz

		Vec3 dFactor = D1 % D2;   // scales x: sy*sz,  y:  sx*sz    z: sx*sy

		// Diagonal Elements      // sx*sx*sx,  sy*sy*sy,  sz*sz*sz
		Vec3 squaredIntegral = elementWiseCube(v0) + elementWiseCube(v1) + elementWiseCube(v2) + elementWiseMul(elementWiseSquare(v0), v1 + v2) + elementWiseMul(elementWiseSquare(v1), v0 + v2) + elementWiseMul(elementWiseSquare(v2), v0 + v1) + elementWiseMul(elementWiseMul(v0, v1), v2);
		Vec3 diagonalElementParts = elementWiseMul(dFactor, squaredIntegral); // (sx^3)*sy*sz, sx*(sy^3)*sz, sx*sy*(sz^3)

		totalDiagElementParts += diagonalElementParts;

		//total[0][0] += diagonalElementParts.y + diagonalElementParts.z; // sx*sy*sz*(sy^2+sz^2)
		//total[1][1] += diagonalElementParts.z + diagonalElementParts.x; // sx*sy*sz*(sz^2+sx^2)
		//total[2][2] += diagonalElementParts.x + diagonalElementParts.y; // sx*sy*sz*(sx^2+sy^2)

		// Other Elements
		double selfProducts  =	v0.x*v0.y*v0.z + v1.x*v1.y*v1.z + v2.x*v2.y*v2.z;
		double twoSames      =	v0.x*v0.y*v1.z + v0.x*v1.y*v0.z + v0.x*v1.y*v1.z + v0.x*v0.y*v2.z + v0.x*v2.y*v0.z + v0.x*v2.y*v2.z +
								v1.x*v0.y*v0.z + v1.x*v1.y*v0.z + v1.x*v0.y*v1.z + v1.x*v1.y*v2.z + v1.x*v2.y*v1.z + v1.x*v2.y*v2.z +
								v2.x*v0.y*v0.z + v2.x*v1.y*v2.z + v2.x*v0.y*v2.z + v2.x*v1.y*v1.z + v2.x*v2.y*v0.z + v2.x*v2.y*v1.z;
		double allDifferents =	v0.x*v1.y*v2.z + v0.x*v2.y*v1.z + v1.x*v0.y*v2.z + v1.x*v2.y*v0.z + v2.x*v0.y*v1.z + v2.x*v1.y*v0.z;

		double xyzIntegral = -(3 * selfProducts + twoSames + allDifferents / 2); // scales linearly by sx*sy*sz

		totalOffDiag += dFactor * xyzIntegral;

		//total[1][0] += dFactor.z * xyzIntegral; // sx*sy*sz* sx*sy
		//total[2][0] += dFactor.y * xyzIntegral; // sx*sy*sz* sx*sz
		//total[2][1] += dFactor.x * xyzIntegral; // sx*sy*sz* sz*sy
	}
	
	return ScalableInertialMatrix(totalDiagElementParts / 60, totalOffDiag / 60);
}


/*
	The total inertial matrix is given by the integral over the volume of the shape of the following matrix:
	[[
	[y^2+z^2,    xy,    xz],
	[xy,    x^2+z^2,    yz],
	[xz,    yz,    x^2+y^2]
	]]

	This has been reworked to a surface integral resulting in the given formulae
*/
SymmetricMat3 Polyhedron::getInertia(const CFrame& reference) const {
	return getScalableInertia(reference).toMatrix();
}

SymmetricMat3 Polyhedron::getInertia(Vec3 reference) const {
	return this->getInertia(CFrame(reference));
}

SymmetricMat3 Polyhedron::getInertia(Mat3 reference) const {
	return this->getInertia(CFrame(reference));
}

SymmetricMat3 Polyhedron::getInertia() const {
	return this->getInertia(CFrame());
}

void Polyhedron::getCircumscribedEllipsoid() const {

}


NormalizedPolyhedron Polyhedron::normalized() const {
	BoundingBox bounds = getBounds();
	Vec3 center = bounds.getCenter();
	DiagonalMat3 scale{2 / bounds.getWidth(), 2 / bounds.getHeight(), 2 / bounds.getDepth()};
	
	Polyhedron scaled = translatedAndScaled(-center, scale);

	double volume = scaled.getVolume();
	Vec3 centerOfMass = scaled.getCenterOfMass();
	ScalableInertialMatrix inertia = scaled.getScalableInertia(CFrame(centerOfMass));

	return NormalizedPolyhedron(std::move(scaled), center, ~scale, volume, centerOfMass, inertia);
}

Polyhedron::operator Shape() const {
	BoundingBox bounds = getBounds();
	return Shape(new NormalizedPolyhedron(this->normalized()), bounds.getWidth(), bounds.getHeight(), bounds.getDepth());
}

Triangle Polyhedron::getTriangle(int index) const {
	size_t offset = getOffset(triangleCount);
	return Triangle{triangles[index], triangles[index + offset], triangles[index + 2 * offset]};
}

IteratorFactory<ShapeVertexIter> Polyhedron::iterVertices() const {
	return IteratorFactory<ShapeVertexIter>(ShapeVertexIter{vertices, getOffset(vertexCount)}, ShapeVertexIter{vertices + vertexCount, getOffset(vertexCount)});
}
IteratorFactory<ShapeTriangleIter> Polyhedron::iterTriangles() const {
	return IteratorFactory<ShapeTriangleIter>(ShapeTriangleIter{triangles, getOffset(triangleCount)}, ShapeTriangleIter{triangles + triangleCount, getOffset(triangleCount)});
}

void Polyhedron::getTriangles(Triangle* triangleBuf) const {
	size_t i = 0;
	for(Triangle triangle : iterTriangles()) {
		triangleBuf[i++] = triangle;
	}
}
void Polyhedron::getVertices(Vec3f* vertexBuf) const {
	size_t i = 0;
	for(Vec3f vertex : iterVertices()) {
		vertexBuf[i++] = vertex;
	}
}

float Polyhedron::getIntersectionDistance(Vec3f origin, Vec3f direction) const {
	const float EPSILON = 0.0000001f;
	float t = INFINITY;
	for (Triangle triangle : iterTriangles()) {
		Vec3f v0 = (*this)[triangle.firstIndex];
		Vec3f v1 = (*this)[triangle.secondIndex];
		Vec3f v2 = (*this)[triangle.thirdIndex];

		Vec3f edge1, edge2, h, s, q;
		float a, f, u, v;

		edge1 = v1 - v0;
		edge2 = v2 - v0;

		h = direction % edge2;
		a = edge1 * h;

		if (a > -EPSILON && a < EPSILON) continue;   
		
		f = 1.0f / a;
		s = origin - v0;
		u = f * (s * h);

		if (u < 0.0 || u > 1.0) continue;
		
		q = s % edge1;
		v = direction * f * q;

		if (v < 0.0f || u + v > 1.0f) continue;

		float r = edge2 * f * q;
		if (r > EPSILON) { 
			if (r < t) t = r;
		} else {
			//Log::debug("Line intersection but not a ray intersection");
			continue;
		}
	}

	return t;
}

