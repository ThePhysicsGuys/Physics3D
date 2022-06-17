#include "triangleMesh.h"
#include "triangleMeshCommon.h"

#include <immintrin.h>

// SSE2 implementation for TriangleMesh functions
namespace P3D {

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

	__m128 xMax = _mm_load_ps(xValues);
	__m128 xMin = xMax;
	__m128 yMax = _mm_load_ps(xValues + BLOCK_WIDTH);
	__m128 yMin = yMax;
	__m128 zMax = _mm_load_ps(xValues + BLOCK_WIDTH * 2);
	__m128 zMin = zMax;

	
	
	__m128 xVal = _mm_load_ps(xValues + 4);
	__m128 yVal = _mm_load_ps(xValues + BLOCK_WIDTH + 4);
	__m128 zVal = _mm_load_ps(xValues + BLOCK_WIDTH * 2 + 4);

	xMax = _mm_max_ps(xMax, xVal);
	yMax = _mm_max_ps(yMax, yVal);
	zMax = _mm_max_ps(zMax, zVal);

	xMin = _mm_min_ps(xMin, xVal);
	yMin = _mm_min_ps(yMin, yVal);
	zMin = _mm_min_ps(zMin, zVal);
	


	const float *verticesEnd = xValues + offset * 3;

	for(const float *verticesPointer = xValues + BLOCK_WIDTH * 3;
	 	verticesPointer != verticesEnd;
	  	verticesPointer += BLOCK_WIDTH * 3) {
		
	
		__m128 xVal = _mm_load_ps(verticesPointer);
		__m128 yVal = _mm_load_ps(verticesPointer + BLOCK_WIDTH);
		__m128 zVal = _mm_load_ps(verticesPointer + BLOCK_WIDTH * 2);

		xMax = _mm_max_ps(xMax, xVal);
		yMax = _mm_max_ps(yMax, yVal);
		zMax = _mm_max_ps(zMax, zVal);

		xMin = _mm_min_ps(xMin, xVal);
		yMin = _mm_min_ps(yMin, yVal);
		zMin = _mm_min_ps(zMin, zVal);
	

		xVal = _mm_load_ps(verticesPointer + 4);
		yVal = _mm_load_ps(verticesPointer + BLOCK_WIDTH + 4);
		zVal = _mm_load_ps(verticesPointer + BLOCK_WIDTH * 2 + 4);

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
	__m128 yVal = _mm_load_ps(xValues + BLOCK_WIDTH);
	__m128 zVal = _mm_load_ps(xValues + BLOCK_WIDTH * 2);

	__m128 xMin = custom_fmadd_ps(_mm_set1_ps(mxz), zVal, custom_fmadd_ps(_mm_set1_ps(mxy), yVal, _mm_mul_ps(_mm_set1_ps(mxx), xVal)));
	__m128 yMin = custom_fmadd_ps(_mm_set1_ps(myz), zVal, custom_fmadd_ps(_mm_set1_ps(myy), yVal, _mm_mul_ps(_mm_set1_ps(myx), xVal)));
	__m128 zMin = custom_fmadd_ps(_mm_set1_ps(mzz), zVal, custom_fmadd_ps(_mm_set1_ps(mzy), yVal, _mm_mul_ps(_mm_set1_ps(mzx), xVal)));

	__m128 xMax = xMin;
	__m128 yMax = yMin;
	__m128 zMax = zMin;

		
	xVal = _mm_load_ps(xValues + 4);
	yVal = _mm_load_ps(xValues + BLOCK_WIDTH + 4);
	zVal = _mm_load_ps(xValues + BLOCK_WIDTH * 2 + 4);

	__m128 dotX = custom_fmadd_ps(_mm_set1_ps(mxz), zVal, custom_fmadd_ps(_mm_set1_ps(mxy), yVal, _mm_mul_ps(_mm_set1_ps(mxx), xVal)));
	xMin = _mm_min_ps(xMin, dotX);
	xMax = _mm_max_ps(xMax, dotX);
	__m128 dotY = custom_fmadd_ps(_mm_set1_ps(myz), zVal, custom_fmadd_ps(_mm_set1_ps(myy), yVal, _mm_mul_ps(_mm_set1_ps(myx), xVal)));
	yMin = _mm_min_ps(yMin, dotY);
	yMax = _mm_max_ps(yMax, dotY);
	__m128 dotZ = custom_fmadd_ps(_mm_set1_ps(mzz), zVal, custom_fmadd_ps(_mm_set1_ps(mzy), yVal, _mm_mul_ps(_mm_set1_ps(mzx), xVal)));
	zMin = _mm_min_ps(zMin, dotZ);
	zMax = _mm_max_ps(zMax, dotZ);


	const float *verticesEnd = xValues + offset * 3;

	for(const float *verticesPointer = xValues + BLOCK_WIDTH * 3;
	 	verticesPointer != verticesEnd;
	  	verticesPointer += BLOCK_WIDTH * 3) {
		

		__m128 xVal = _mm_load_ps(verticesPointer);
		__m128 yVal = _mm_load_ps(verticesPointer + BLOCK_WIDTH);
		__m128 zVal = _mm_load_ps(verticesPointer + BLOCK_WIDTH * 2);

		__m128 dotX = custom_fmadd_ps(_mm_set1_ps(mxz), zVal, custom_fmadd_ps(_mm_set1_ps(mxy), yVal, _mm_mul_ps(_mm_set1_ps(mxx), xVal)));
		xMin = _mm_min_ps(xMin, dotX);
		xMax = _mm_max_ps(xMax, dotX);
		__m128 dotY = custom_fmadd_ps(_mm_set1_ps(myz), zVal, custom_fmadd_ps(_mm_set1_ps(myy), yVal, _mm_mul_ps(_mm_set1_ps(myx), xVal)));
		yMin = _mm_min_ps(yMin, dotY);
		yMax = _mm_max_ps(yMax, dotY);
		__m128 dotZ = custom_fmadd_ps(_mm_set1_ps(mzz), zVal, custom_fmadd_ps(_mm_set1_ps(mzy), yVal, _mm_mul_ps(_mm_set1_ps(mzx), xVal)));
		zMin = _mm_min_ps(zMin, dotZ);
		zMax = _mm_max_ps(zMax, dotZ);



		xVal = _mm_load_ps(verticesPointer + 4);
		yVal = _mm_load_ps(verticesPointer + BLOCK_WIDTH + 4);
		zVal = _mm_load_ps(verticesPointer + BLOCK_WIDTH * 2 + 4);

		dotX = custom_fmadd_ps(_mm_set1_ps(mxz), zVal, custom_fmadd_ps(_mm_set1_ps(mxy), yVal, _mm_mul_ps(_mm_set1_ps(mxx), xVal)));
		xMin = _mm_min_ps(xMin, dotX);
		xMax = _mm_max_ps(xMax, dotX);
		dotY = custom_fmadd_ps(_mm_set1_ps(myz), zVal, custom_fmadd_ps(_mm_set1_ps(myy), yVal, _mm_mul_ps(_mm_set1_ps(myx), xVal)));
		yMin = _mm_min_ps(yMin, dotY);
		yMax = _mm_max_ps(yMax, dotY);
		dotZ = custom_fmadd_ps(_mm_set1_ps(mzz), zVal, custom_fmadd_ps(_mm_set1_ps(mzy), yVal, _mm_mul_ps(_mm_set1_ps(mzx), xVal)));
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
	
	__m128 bestDot = custom_fmadd_ps(dz, _mm_load_ps(xValues + BLOCK_WIDTH * 2), custom_fmadd_ps(dy, _mm_load_ps(xValues + BLOCK_WIDTH), _mm_mul_ps(dx, _mm_load_ps(xValues))));

	__m128i bestIndices = _mm_set1_epi32(0);

	unsigned int blockI = 1;
	
	__m128i indices = _mm_set1_epi32(static_cast<int>(blockI));

	__m128 dot = custom_fmadd_ps(dz, _mm_load_ps(xValues + BLOCK_WIDTH * 2 + 4), custom_fmadd_ps(dy, _mm_load_ps(xValues + BLOCK_WIDTH + 4), _mm_mul_ps(dx, _mm_load_ps(xValues + 4))));

		//Compare greater than, returns false if either operand is NaN.
	__m128 whichAreMax = _mm_cmpgt_ps(dot, bestDot);

	bestDot = custom_blendv_ps(bestDot, dot, whichAreMax);
	bestIndices = custom_blendv_epi32(bestIndices, indices, _mm_castps_si128(whichAreMax));

	blockI++;



	const float *verticesEnd = xValues + offset * 3;
	
	for(const float *verticesPointer = xValues + BLOCK_WIDTH * 3;
	 	verticesPointer != verticesEnd;
	  	verticesPointer += BLOCK_WIDTH * 3) {


		__m128i indices = _mm_set1_epi32(static_cast<int>(blockI));

		__m128 dot = custom_fmadd_ps(dz, _mm_load_ps(verticesPointer + BLOCK_WIDTH * 2), custom_fmadd_ps(dy, _mm_load_ps(verticesPointer + BLOCK_WIDTH), _mm_mul_ps(dx, _mm_load_ps(verticesPointer))));

		//Compare greater than, returns false if either operand is NaN.
		__m128 whichAreMax = _mm_cmpgt_ps(dot, bestDot);

		bestDot = custom_blendv_ps(bestDot, dot, whichAreMax);
		bestIndices = custom_blendv_epi32(bestIndices, indices, _mm_castps_si128(whichAreMax));

		blockI++;


		indices = _mm_set1_epi32(static_cast<int>(blockI));

		dot = custom_fmadd_ps(dz, _mm_load_ps(verticesPointer + BLOCK_WIDTH * 2 + 4), custom_fmadd_ps(dy, _mm_load_ps(verticesPointer + BLOCK_WIDTH + 4), _mm_mul_ps(dx, _mm_load_ps(verticesPointer + 4))));

		//Compare greater than, returns false if either operand is NaN.
		whichAreMax = _mm_cmpgt_ps(dot, bestDot);

		bestDot = custom_blendv_ps(bestDot, dot, whichAreMax);
		bestIndices = custom_blendv_epi32(bestIndices, indices, _mm_castps_si128(whichAreMax));

		blockI++;

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
	__m128 bestX1 = _mm_load_ps(xValues);
	__m128 bestY1 = _mm_load_ps(xValues + BLOCK_WIDTH);
	__m128 bestZ1 = _mm_load_ps(xValues + BLOCK_WIDTH * 2);

	__m128 bestX2 = bestX1;
	__m128 bestY2 = bestY1;
	__m128 bestZ2 = bestZ1;

	__m128 bestDot1 = custom_fmadd_ps(dz, bestZ1, custom_fmadd_ps(dy, bestY1, _mm_mul_ps(dx, bestX1)));

	__m128 bestDot2 = bestDot1;



	__m128 xVal1 = _mm_load_ps(xValues + 4);
	__m128 yVal1 = _mm_load_ps(xValues + BLOCK_WIDTH + 4);
	__m128 zVal1 = _mm_load_ps(xValues + BLOCK_WIDTH * 2 + 4);

	__m128 xVal2 = xVal1;
	__m128 yVal2 = yVal1;
	__m128 zVal2 = zVal1;

	__m128 dot1 = custom_fmadd_ps(dz, zVal1, custom_fmadd_ps(dy, yVal1, _mm_mul_ps(dx, xVal1)));

	__m128 dot2 = dot1;

		//Compare greater than, returns false if either operand is NaN.
	__m128 whichAreMax1 = _mm_cmpgt_ps(dot1, bestDot1);

		//Compare greater than, returns false if either operand is NaN.
	__m128 whichAreMax2 = whichAreMax1;

	bestDot1 = custom_blendv_ps(bestDot1, dot1, whichAreMax1);
	bestX1 = custom_blendv_ps(bestX1, xVal1, whichAreMax1);
	bestY1 = custom_blendv_ps(bestY1, yVal1, whichAreMax1);
	bestZ1 = custom_blendv_ps(bestZ1, zVal1, whichAreMax1);


	const float *verteciesEnd = xValues + offset * 3;

	for(const float *verticesPointer = xValues + BLOCK_WIDTH * 3;
		verticesPointer != verteciesEnd;
		verticesPointer += BLOCK_WIDTH * 3) {
		//__m128i indices = _mm_set1_epi32(static_cast<int>(blockI));

		xVal1 = _mm_load_ps(verticesPointer);
		yVal1 = _mm_load_ps(verticesPointer + BLOCK_WIDTH);
		zVal1 = _mm_load_ps(verticesPointer + BLOCK_WIDTH * 2);

		xVal2 = _mm_load_ps(verticesPointer + 4);
		yVal2 = _mm_load_ps(verticesPointer + BLOCK_WIDTH + 4);
		zVal2 = _mm_load_ps(verticesPointer + BLOCK_WIDTH * 2 + 4);


		dot1 = custom_fmadd_ps(dz, zVal1, custom_fmadd_ps(dy, yVal1, _mm_mul_ps(dx, xVal1)));

		dot2 = custom_fmadd_ps(dz, zVal2, custom_fmadd_ps(dy, yVal2, _mm_mul_ps(dx, xVal2)));

		//Compare greater than, returns false if either operand is NaN.
		whichAreMax1 = _mm_cmpgt_ps(bestDot1, dot1);
		whichAreMax2 = _mm_cmpgt_ps(bestDot2, dot2);

		bestDot1 = custom_blendv_ps(dot1, bestDot1, whichAreMax1);
		bestDot2 = custom_blendv_ps(dot2, bestDot2, whichAreMax2);
		bestX1 = custom_blendv_ps(xVal1, bestX1, whichAreMax1);
		bestX2 = custom_blendv_ps(xVal2, bestX2, whichAreMax2);
		bestY1 = custom_blendv_ps(yVal1, bestY1, whichAreMax1);
		bestY2 = custom_blendv_ps(yVal2, bestY2, whichAreMax2);
		bestZ1 = custom_blendv_ps(zVal1, bestZ1, whichAreMax1);
		bestZ2 = custom_blendv_ps(zVal2, bestZ2, whichAreMax2);



	}

			//Compare greater than, returns false if either operand is NaN.
		__m128 whichAreMax = _mm_cmpgt_ps(bestDot1, bestDot2);

		__m128 bestDot = custom_blendv_ps(bestDot2, bestDot1, whichAreMax);
		__m128 bestX = custom_blendv_ps(bestX2, bestX1, whichAreMax);
		__m128 bestY = custom_blendv_ps(bestY2, bestY1, whichAreMax);
		__m128 bestZ = custom_blendv_ps(bestZ2, bestZ1, whichAreMax);


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