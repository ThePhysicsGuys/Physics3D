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

	__m128 dot1 = custom_fmadd_ps(dz, zVal1, custom_fmadd_ps(dy, yVal1, _mm_mul_ps(dx, xVal1)));

	__m128 dot2 = dot1;

	//Compare greater than, returns false if either operand is NaN.
	__m128 whichAreMax1 = _mm_cmpgt_ps(bestDot1, dot1);

	//Compare greater than, returns false if either operand is NaN.
	__m128 whichAreMax2 = whichAreMax1;

	bestDot1 = _mm_blendv_ps(dot1, bestDot1, whichAreMax1);
	bestX1 = _mm_blendv_ps(xVal1, bestX1, whichAreMax1);
	bestY1 = _mm_blendv_ps(yVal1, bestY1, whichAreMax1);
	bestZ1 = _mm_blendv_ps(zVal1, bestZ1, whichAreMax1); 
	

	const float *verticesEnd = xValues + offset * 3;

	for(const float *verticesPointer = xValues + BLOCK_WIDTH * 3;
	 	verticesPointer != verticesEnd;
	  	verticesPointer += BLOCK_WIDTH * 3) {
		//__m128i indices = _mm_set1_epi32(static_cast<int>(blockI));

		__m128 xVal1 = _mm_load_ps(verticesPointer);
		__m128 yVal1 = _mm_load_ps(verticesPointer + BLOCK_WIDTH);
		__m128 zVal1 = _mm_load_ps(verticesPointer + BLOCK_WIDTH * 2);

		__m128 xVal2 = _mm_load_ps(verticesPointer + 4);
		__m128 yVal2 = _mm_load_ps(verticesPointer + BLOCK_WIDTH + 4);
		__m128 zVal2 = _mm_load_ps(verticesPointer + BLOCK_WIDTH * 2 + 4);


		dot1 = custom_fmadd_ps(dz, zVal1, custom_fmadd_ps(dy, yVal1, _mm_mul_ps(dx, xVal1)));
		dot2 = custom_fmadd_ps(dz, zVal2, custom_fmadd_ps(dy, yVal2, _mm_mul_ps(dx, xVal2)));
	
		//Compare greater than, returns false if either operand is NaN.
		whichAreMax1 = _mm_cmpgt_ps(bestDot1, dot1);
		//Compare greater than, returns false if either operand is NaN.
		whichAreMax2 = _mm_cmpgt_ps(bestDot2, dot2);


		bestDot1 = _mm_blendv_ps(dot1, bestDot1, whichAreMax1);
		bestDot2 = _mm_blendv_ps(dot2, bestDot2, whichAreMax2);
		bestX1 = _mm_blendv_ps(xVal1, bestX1, whichAreMax1);
		bestX2 = _mm_blendv_ps(xVal2, bestX2, whichAreMax2);
		bestY1 = _mm_blendv_ps(yVal1, bestY1, whichAreMax1);
		bestY2 = _mm_blendv_ps(yVal2, bestY2, whichAreMax2);
		bestZ1 = _mm_blendv_ps(zVal1, bestZ1, whichAreMax1);
		bestZ2 = _mm_blendv_ps(zVal2, bestZ2, whichAreMax2);
	
	
	}

		//Compare greater than, returns false if either operand is NaN.
		__m128 whichAreMax = _mm_cmpgt_ps(bestDot1, bestDot2);


		__m128 bestDot = _mm_blendv_ps(bestDot2, bestDot1, whichAreMax);
		__m128 bestX = _mm_blendv_ps(bestX2, bestX1, whichAreMax);
		__m128 bestY = _mm_blendv_ps(bestY2, bestY1, whichAreMax);
		__m128 bestZ = _mm_blendv_ps(bestZ2, bestZ1, whichAreMax);
		

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