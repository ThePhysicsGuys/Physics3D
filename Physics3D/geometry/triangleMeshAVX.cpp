#include "triangleMesh.h"

#include <immintrin.h>

// AVX2 implementation for TriangleMesh functions
namespace P3D {
inline static size_t getOffset(size_t size) {
	return (size + 7) & 0xFFFFFFFFFFFFFFF8;
}

#ifdef _MSC_VER
inline static uint32_t countZeros(uint32_t x) {
	unsigned long ret;
	_BitScanForward(&ret, x);
	return (int) ret;
}
#else
inline static uint32_t countZeros(uint32_t x) {
	return __builtin_ctz(x);
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

int TriangleMesh::furthestIndexInDirectionAVX(const Vec3f& direction) const {
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

Vec3f TriangleMesh::furthestInDirectionAVX(const Vec3f& direction) const {
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
inline static BoundingBox toBounds(__m256 xMin, __m256 xMax, __m256 yMin, __m256 yMax, __m256 zMin, __m256 zMax) {
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

BoundingBox TriangleMesh::getBoundsAVX() const {
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

BoundingBox TriangleMesh::getBoundsAVX(const Mat3f& referenceFrame) const {
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
};