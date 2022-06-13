#include "triangleMesh.h"
#include "triangleMeshCommon.h"

#include <immintrin.h>

// SSE2 implementation for TriangleMesh functions
namespace P3D {
// SSE4-specific implementation, the _mm_blendv_ps instruction was the bottleneck

int TriangleMesh::furthestIndexInDirectionSSE4(const Vec3f& direction) const {
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

	bestDot = _mm_blendv_ps(bestDot, dot, whichAreMax);
	bestIndices = _mm_blendv_epi8(bestIndices, indices, _mm_castps_si128(whichAreMax));
	
	blockI++;



	const float *verticesEnd = xValues + offset * 3;

	for(const float *verticesPointer = xValues + BLOCK_WIDTH * 3;
	 	verticesPointer != verticesEnd;
	  	verticesPointer += BLOCK_WIDTH * 3) {
		__m128i indices = _mm_set1_epi32(static_cast<int>(blockI));

		__m128 dot = custom_fmadd_ps(dz, _mm_load_ps(verticesPointer + BLOCK_WIDTH * 2), custom_fmadd_ps(dy, _mm_load_ps(verticesPointer + BLOCK_WIDTH), _mm_mul_ps(dx, _mm_load_ps(verticesPointer))));

		//Compare greater than, returns false if either operand is NaN.
		__m128 whichAreMax = _mm_cmpgt_ps(dot, bestDot);

		bestDot = _mm_blendv_ps(bestDot, dot, whichAreMax);
		bestIndices = _mm_blendv_epi8(bestIndices, indices, _mm_castps_si128(whichAreMax));
	
		blockI++;


		indices = _mm_set1_epi32(static_cast<int>(blockI));

		dot = custom_fmadd_ps(dz, _mm_load_ps(verticesPointer + BLOCK_WIDTH * 2 + 4), custom_fmadd_ps(dy, _mm_load_ps(verticesPointer + BLOCK_WIDTH + 4), _mm_mul_ps(dx, _mm_load_ps(verticesPointer + 4))));

		//Compare greater than, returns false if either operand is NaN.
		whichAreMax = _mm_cmpgt_ps(dot, bestDot);

		bestDot = _mm_blendv_ps(bestDot, dot, whichAreMax);
		bestIndices = _mm_blendv_epi8(bestIndices, indices, _mm_castps_si128(whichAreMax));
	
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

Vec3f TriangleMesh::furthestInDirectionSSE4(const Vec3f& direction) const {
	size_t vertexCount = this->vertexCount;

	__m128 dx = _mm_set1_ps(direction.x);
	__m128 dy = _mm_set1_ps(direction.y);
	__m128 dz = _mm_set1_ps(direction.z);

	size_t offset = getOffset(vertexCount);
	const float* xValues = this->vertices;

	__m128 bestX = _mm_load_ps(xValues);
	__m128 bestY = _mm_load_ps(xValues + BLOCK_WIDTH);
	__m128 bestZ = _mm_load_ps(xValues + BLOCK_WIDTH * 2);

	__m128 bestDot = custom_fmadd_ps(dz, bestZ, custom_fmadd_ps(dy, bestY, _mm_mul_ps(dx, bestX)));

	__m128 xVal = _mm_load_ps(xValues + 4);
	__m128 yVal = _mm_load_ps(xValues + BLOCK_WIDTH + 4);
	__m128 zVal = _mm_load_ps(xValues + BLOCK_WIDTH * 2 + 4);

	__m128 dot = custom_fmadd_ps(dz, zVal, custom_fmadd_ps(dy, yVal, _mm_mul_ps(dx, xVal)));

	//Compare greater than, returns false if either operand is NaN.
	__m128 whichAreMax = _mm_cmpgt_ps(dot, bestDot);

	bestDot = _mm_blendv_ps(bestDot, dot, whichAreMax);
	bestX = _mm_blendv_ps(bestX, xVal, whichAreMax);
	bestY = _mm_blendv_ps(bestY, yVal, whichAreMax);
	bestZ = _mm_blendv_ps(bestZ, zVal, whichAreMax);
	

	const float *verticesEnd = xValues + offset * 3;

	for(const float *verticesPointer = xValues + BLOCK_WIDTH * 3;
	 	verticesPointer != verticesEnd;
	  	verticesPointer += BLOCK_WIDTH * 3) {
		//__m128i indices = _mm_set1_epi32(static_cast<int>(blockI));

		__m128 xVal = _mm_load_ps(verticesPointer);
		__m128 yVal = _mm_load_ps(verticesPointer + BLOCK_WIDTH);
		__m128 zVal = _mm_load_ps(verticesPointer + BLOCK_WIDTH * 2);

		__m128 dot = custom_fmadd_ps(dz, zVal, custom_fmadd_ps(dy, yVal, _mm_mul_ps(dx, xVal)));

		//Compare greater than, returns false if either operand is NaN.
		__m128 whichAreMax = _mm_cmpgt_ps(dot, bestDot);

		bestDot = _mm_blendv_ps(bestDot, dot, whichAreMax);
		bestX = _mm_blendv_ps(bestX, xVal, whichAreMax);
		bestY = _mm_blendv_ps(bestY, yVal, whichAreMax);
		bestZ = _mm_blendv_ps(bestZ, zVal, whichAreMax);
	
	
		xVal = _mm_load_ps(verticesPointer + 4);
		yVal = _mm_load_ps(verticesPointer + BLOCK_WIDTH + 4);
		zVal = _mm_load_ps(verticesPointer + BLOCK_WIDTH * 2 + 4);

		dot = custom_fmadd_ps(dz, zVal, custom_fmadd_ps(dy, yVal, _mm_mul_ps(dx, xVal)));

		//Compare greater than, returns false if either operand is NaN.
		whichAreMax = _mm_cmpgt_ps(dot, bestDot);

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