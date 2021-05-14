#include "triangleMesh.h"

// SSE2 implementation for TriangleMesh functions
namespace P3D {
inline  size_t getOffset(size_t size) {
	return (size + 7) & 0xFFFFFFFFFFFFFFF8;
}
};

#include <immintrin.h>
namespace P3D {
#ifdef _MSC_VER
#define GET_SSE_ELEM(reg, index) reg.m128_f32[index]
#define GET_SSE_ELEMi(reg, index) reg.m128i_i32[index]
#else
#define GET_SSE_ELEM(reg, index) reg[index]

int mm_extractv_epi32(__m128i a, int b) {
	alignas(16) int buf[4];
	_mm_storeu_si128(reinterpret_cast<__m128i*>(buf), a);
	return buf[b];
}

#define GET_SSE_ELEMi(reg, index) mm_extractv_epi32(reg, index)
#endif
};

namespace P3D {
#define SWAP_2x2 0b01001110
#define SWAP_1x1 0b10110001

#ifdef _MSC_VER
inline  uint32_t countZeros(uint32_t mask) {
	unsigned long ret = 0;
	_BitScanForward(&ret, mask);
	return static_cast<int>(ret);
}
#else
inline  uint32_t countZeros(uint32_t mask) {
	return __builtin_ctz(mask);
}
#endif


static __m128 custom_fmadd_ps(__m128 a, __m128 b, __m128 c) {
	return  _mm_add_ps(_mm_mul_ps(a, b), c);
}

// used if SSE4_1 is not available, emulates _mm_blendv_ps
static __m128 custom_blendv_ps(__m128 a, __m128 b, __m128 mask) {
	return  _mm_or_ps(_mm_andnot_ps(mask, a), _mm_and_ps(mask, b));
}

// used if SSE4_1 is not available, emulates _mm_blendv_epi32
static __m128i custom_blendv_epi32(__m128i a, __m128i b, __m128i mask) {
	return _mm_or_si128(_mm_andnot_si128(mask, a), _mm_and_si128(mask, b));
}

static BoundingBox toBounds(__m128 xMin, __m128 xMax, __m128 yMin, __m128 yMax, __m128 zMin, __m128 zMax) {

	for(int i = 0; i < 3; i++) {
		__m128 xShuf = _mm_shuffle_ps(xMax, xMax, 0x93);
		xMax = _mm_max_ps(xMax, xShuf);
	}
	for(int i = 0; i < 3; i++) {
		__m128 yShuf = _mm_shuffle_ps(yMax, yMax, 0x93);
		yMax = _mm_max_ps(yMax, yShuf);
	}
	for(int i = 0; i < 3; i++) {
		__m128 zShuf = _mm_shuffle_ps(zMax, zMax, 0x93);
		zMax = _mm_max_ps(zMax, zShuf);
	}


	for(int i = 0; i < 3; i++) {
		__m128 xShuf = _mm_shuffle_ps(xMin, xMin, 0x93);
		xMin = _mm_min_ps(xMin, xShuf);
	}
	for(int i = 0; i < 3; i++) {
		__m128 yShuf = _mm_shuffle_ps(yMin, yMin, 0x93);
		yMin = _mm_min_ps(yMin, yShuf);
	}
	for(int i = 0; i < 3; i++) {
		__m128 zShuf = _mm_shuffle_ps(zMin, zMin, 0x93);
		zMin = _mm_min_ps(zMin, zShuf);
	}

	return BoundingBox{GET_SSE_ELEM(xMin, 0), GET_SSE_ELEM(yMin, 0), GET_SSE_ELEM(zMin, 0), GET_SSE_ELEM(xMax, 0), GET_SSE_ELEM(yMax, 0), GET_SSE_ELEM(zMax, 0)};
}

BoundingBox TriangleMesh::getBoundsSSE() const {
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

	for(size_t blockI = 1; blockI < (vertexCount + 3) / 4; blockI++) {
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

BoundingBox TriangleMesh::getBoundsSSE(const Mat3f& referenceFrame) const {
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

	__m128 xMin = custom_fmadd_ps(_mm_set1_ps(mxz), zVal, custom_fmadd_ps(_mm_set1_ps(mxy), yVal, _mm_mul_ps(_mm_set1_ps(mxx), xVal)));
	__m128 yMin = custom_fmadd_ps(_mm_set1_ps(myz), zVal, custom_fmadd_ps(_mm_set1_ps(myy), yVal, _mm_mul_ps(_mm_set1_ps(myx), xVal)));
	__m128 zMin = custom_fmadd_ps(_mm_set1_ps(mzz), zVal, custom_fmadd_ps(_mm_set1_ps(mzy), yVal, _mm_mul_ps(_mm_set1_ps(mzx), xVal)));

	__m128 xMax = xMin;
	__m128 yMax = yMin;
	__m128 zMax = zMin;

	for(size_t blockI = 1; blockI < (vertexCount + 3) / 4; blockI++) {
		__m128 xVal = _mm_load_ps(xValues + blockI * 4);
		__m128 yVal = _mm_load_ps(yValues + blockI * 4);
		__m128 zVal = _mm_load_ps(zValues + blockI * 4);

		__m128 dotX = custom_fmadd_ps(_mm_set1_ps(mxz), zVal, custom_fmadd_ps(_mm_set1_ps(mxy), yVal, _mm_mul_ps(_mm_set1_ps(mxx), xVal)));
		xMin = _mm_min_ps(xMin, dotX);
		xMax = _mm_max_ps(xMax, dotX);
		__m128 dotY = custom_fmadd_ps(_mm_set1_ps(myz), zVal, custom_fmadd_ps(_mm_set1_ps(myy), yVal, _mm_mul_ps(_mm_set1_ps(myx), xVal)));
		yMin = _mm_min_ps(yMin, dotY);
		yMax = _mm_max_ps(yMax, dotY);
		__m128 dotZ = custom_fmadd_ps(_mm_set1_ps(mzz), zVal, custom_fmadd_ps(_mm_set1_ps(mzy), yVal, _mm_mul_ps(_mm_set1_ps(mzx), xVal)));
		zMin = _mm_min_ps(zMin, dotZ);
		zMax = _mm_max_ps(zMax, dotZ);
	}

	return toBounds(xMin, xMax, yMin, yMax, zMin, zMax);
}

int TriangleMesh::furthestIndexInDirectionSSE(const Vec3f& direction) const {
	size_t vertexCount = this->vertexCount;

	__m128 dx = _mm_set1_ps(direction.x);
	__m128 dy = _mm_set1_ps(direction.y);
	__m128 dz = _mm_set1_ps(direction.z);

	size_t offset = getOffset(vertexCount);

	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	__m128 bestDot = custom_fmadd_ps(dz, _mm_load_ps(zValues), custom_fmadd_ps(dy, _mm_load_ps(yValues), _mm_mul_ps(dx, _mm_load_ps(xValues))));

	__m128i bestIndices = _mm_set1_epi32(0);

	for(size_t blockI = 1; blockI < (vertexCount + 3) / 4; blockI++) {
		__m128i indices = _mm_set1_epi32(static_cast<int>(blockI));

		__m128 dot = custom_fmadd_ps(dz, _mm_load_ps(zValues + blockI * 4), custom_fmadd_ps(dy, _mm_load_ps(yValues + blockI * 4), _mm_mul_ps(dx, _mm_load_ps(xValues + blockI * 4))));

		//Compare greater than, returns false if either operand is NaN.
		__m128 whichAreMax = _mm_cmpgt_ps(dot, bestDot);

		bestDot = custom_blendv_ps(bestDot, dot, whichAreMax);
		bestIndices = custom_blendv_epi32(bestIndices, indices, _mm_castps_si128(whichAreMax));

	}
	__m128 swap4x4 = _mm_shuffle_ps(bestDot, bestDot, 1);
	__m128 bestDotInternalMax = _mm_max_ps(bestDot, swap4x4);
	__m128 swap2x2 = _mm_shuffle_ps(bestDotInternalMax, bestDotInternalMax, SWAP_2x2);
	bestDotInternalMax = _mm_max_ps(bestDotInternalMax, swap2x2);
	__m128 swap1x1 = _mm_shuffle_ps(bestDotInternalMax, bestDotInternalMax, SWAP_1x1);
	bestDotInternalMax = _mm_max_ps(bestDotInternalMax, swap1x1);

	//Compare equality, returns true if either operand is NaN.
	__m128 compare = _mm_cmpeq_ps(bestDotInternalMax, bestDot);
	compare = _mm_cmpunord_ps(compare, compare);

	uint32_t mask = _mm_movemask_ps(compare);
	assert(mask != 0);
	uint32_t index = countZeros(mask);
	auto block = GET_SSE_ELEMi(bestIndices, index);
	return block * 4 + index;
}

Vec3f TriangleMesh::furthestInDirectionSSE(const Vec3f& direction) const {
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

	__m128 bestDot = custom_fmadd_ps(dz, bestZ, custom_fmadd_ps(dy, bestY, _mm_mul_ps(dx, bestX)));

	for(size_t blockI = 1; blockI < (vertexCount + 3) / 4; blockI++) {
		__m128i indices = _mm_set1_epi32(static_cast<int>(blockI));

		__m128 xVal = _mm_load_ps(xValues + blockI * 4);
		__m128 yVal = _mm_load_ps(yValues + blockI * 4);
		__m128 zVal = _mm_load_ps(zValues + blockI * 4);

		__m128 dot = custom_fmadd_ps(dz, zVal, custom_fmadd_ps(dy, yVal, _mm_mul_ps(dx, xVal)));

		//Compare greater than, returns false if either operand is NaN.
		__m128 whichAreMax = _mm_cmpgt_ps(dot, bestDot);

		bestDot = custom_blendv_ps(bestDot, dot, whichAreMax);
		bestX = custom_blendv_ps(bestX, xVal, whichAreMax);
		bestY = custom_blendv_ps(bestY, yVal, whichAreMax);
		bestZ = custom_blendv_ps(bestZ, zVal, whichAreMax);
	}

	__m128 swap4x4 = _mm_shuffle_ps(bestDot, bestDot, 1);
	__m128 bestDotInternalMax = _mm_max_ps(bestDot, swap4x4);
	__m128 swap2x2 = _mm_shuffle_ps(bestDotInternalMax, bestDotInternalMax, SWAP_2x2);
	bestDotInternalMax = _mm_max_ps(bestDotInternalMax, swap2x2);
	__m128 swap1x1 = _mm_shuffle_ps(bestDotInternalMax, bestDotInternalMax, SWAP_1x1);
	bestDotInternalMax = _mm_max_ps(bestDotInternalMax, swap1x1);

	//Compare equality, return true if either operand is NaN.
	__m128 compare = _mm_cmpeq_ps(bestDotInternalMax, bestDot);
	compare = _mm_cmpunord_ps(compare, compare);

	uint32_t mask = _mm_movemask_ps(compare);
	assert(mask != 0);
	uint32_t index = countZeros(mask);
	return Vec3f(GET_SSE_ELEM(bestX, index), GET_SSE_ELEM(bestY, index), GET_SSE_ELEM(bestZ, index));
}

// SSE4-specific implementation, the _mm_blendv_ps instruction was the bottleneck

int TriangleMesh::furthestIndexInDirectionSSE4(const Vec3f& direction) const {
	size_t vertexCount = this->vertexCount;

	__m128 dx = _mm_set1_ps(direction.x);
	__m128 dy = _mm_set1_ps(direction.y);
	__m128 dz = _mm_set1_ps(direction.z);

	size_t offset = getOffset(vertexCount);

	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	__m128 bestDot = custom_fmadd_ps(dz, _mm_load_ps(zValues), custom_fmadd_ps(dy, _mm_load_ps(yValues), _mm_mul_ps(dx, _mm_load_ps(xValues))));

	__m128i bestIndices = _mm_set1_epi32(0);

	for(size_t blockI = 1; blockI < (vertexCount + 3) / 4; blockI++) {
		__m128i indices = _mm_set1_epi32(static_cast<int>(blockI));

		__m128 dot = custom_fmadd_ps(dz, _mm_load_ps(zValues + blockI * 4), custom_fmadd_ps(dy, _mm_load_ps(yValues + blockI * 4), _mm_mul_ps(dx, _mm_load_ps(xValues + blockI * 4))));

		//Compare greater than, returns false if either operand is NaN.
		__m128 whichAreMax = _mm_cmpgt_ps(dot, bestDot);

		bestDot = _mm_blendv_ps(bestDot, dot, whichAreMax);
		bestIndices = _mm_blendv_epi8(bestIndices, indices, _mm_castps_si128(whichAreMax));
	}
	__m128 swap4x4 = _mm_shuffle_ps(bestDot, bestDot, 1);
	__m128 bestDotInternalMax = _mm_max_ps(bestDot, swap4x4);
	__m128 swap2x2 = _mm_shuffle_ps(bestDotInternalMax, bestDotInternalMax, SWAP_2x2);
	bestDotInternalMax = _mm_max_ps(bestDotInternalMax, swap2x2);
	__m128 swap1x1 = _mm_shuffle_ps(bestDotInternalMax, bestDotInternalMax, SWAP_1x1);
	bestDotInternalMax = _mm_max_ps(bestDotInternalMax, swap1x1);

	//Compare equality, returns true if either operand is NaN.
	__m128 compare = _mm_cmpeq_ps(bestDotInternalMax, bestDot);
	compare = _mm_cmpunord_ps(compare, compare);

	uint32_t mask = _mm_movemask_ps(compare);
	assert(mask != 0);
	uint32_t index = countZeros(mask);
	auto block = GET_SSE_ELEMi(bestIndices, index);
	return block * 4 + index;
}

Vec3f TriangleMesh::furthestInDirectionSSE4(const Vec3f& direction) const {
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

	__m128 bestDot = custom_fmadd_ps(dz, bestZ, custom_fmadd_ps(dy, bestY, _mm_mul_ps(dx, bestX)));

	for(size_t blockI = 1; blockI < (vertexCount + 3) / 4; blockI++) {
		__m128i indices = _mm_set1_epi32(static_cast<int>(blockI));

		__m128 xVal = _mm_load_ps(xValues + blockI * 4);
		__m128 yVal = _mm_load_ps(yValues + blockI * 4);
		__m128 zVal = _mm_load_ps(zValues + blockI * 4);

		__m128 dot = custom_fmadd_ps(dz, zVal, custom_fmadd_ps(dy, yVal, _mm_mul_ps(dx, xVal)));

		//Compare greater than, returns false if either operand is NaN.
		__m128 whichAreMax = _mm_cmpgt_ps(dot, bestDot);

		bestDot = _mm_blendv_ps(bestDot, dot, whichAreMax);
		bestX = _mm_blendv_ps(bestX, xVal, whichAreMax);
		bestY = _mm_blendv_ps(bestY, yVal, whichAreMax);
		bestZ = _mm_blendv_ps(bestZ, zVal, whichAreMax);
	}

	__m128 swap4x4 = _mm_shuffle_ps(bestDot, bestDot, 1);
	__m128 bestDotInternalMax = _mm_max_ps(bestDot, swap4x4);
	__m128 swap2x2 = _mm_shuffle_ps(bestDotInternalMax, bestDotInternalMax, SWAP_2x2);
	bestDotInternalMax = _mm_max_ps(bestDotInternalMax, swap2x2);
	__m128 swap1x1 = _mm_shuffle_ps(bestDotInternalMax, bestDotInternalMax, SWAP_1x1);
	bestDotInternalMax = _mm_max_ps(bestDotInternalMax, swap1x1);

	//Compare equality, return true if either operand is NaN.
	__m128 compare = _mm_cmpeq_ps(bestDotInternalMax, bestDot);
	compare = _mm_cmpunord_ps(compare, compare);

	uint32_t mask = _mm_movemask_ps(compare);
	assert(mask != 0);
	uint32_t index = countZeros(mask);
	return Vec3f(GET_SSE_ELEM(bestX, index), GET_SSE_ELEM(bestY, index), GET_SSE_ELEM(bestZ, index));
}
};