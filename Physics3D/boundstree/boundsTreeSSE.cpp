#include "boundsTree.h" 

#include "../datastructures/alignedPtr.h"
#include <immintrin.h>
 
namespace P3D {

OverlapMatrix TrunkSIMDHelperFallback::computeBoundsOverlapMatrixSSE(const TreeTrunk& trunkA, int trunkASize, const TreeTrunk& trunkB, int trunkBSize) {
	OverlapMatrix result;

    __m128 rx0 = _mm_set1_ps(0);
    __m128 ry0 = _mm_set1_ps(0);
    __m128 rz0 = _mm_set1_ps(0);

    __m128 rx1 = _mm_set1_ps(0);
    __m128 ry1 = _mm_set1_ps(0);
    __m128 rz1 = _mm_set1_ps(0);
    
    alignas(32) float tempBuff[4];




	for(int a = 0; a < trunkASize; a++) {
		
    	BoundsTemplate<float> aBounds0 = trunkA.getBoundsOfSubNode(a);
     
        __m128 axmin = _mm_set1_ps(aBounds0.min.x);

        __m128 aymin = _mm_set1_ps(aBounds0.min.y);

        __m128 azmin = _mm_set1_ps(aBounds0.min.z);

        __m128 axmax = _mm_set1_ps(aBounds0.max.x);

        __m128 aymax = _mm_set1_ps(aBounds0.max.y);

        __m128 azmax = _mm_set1_ps(aBounds0.max.z);


        for(unsigned short int i=0; i<2; i++){

            __m128 bxmin = _mm_load_ps(trunkB.subNodeBounds.xMin + i * 4);
    
            __m128 bymin = _mm_load_ps(trunkB.subNodeBounds.yMin + i * 4);
    
            __m128 bzmin = _mm_load_ps(trunkB.subNodeBounds.zMin + i * 4);
    
            __m128 bxmax = _mm_load_ps(trunkB.subNodeBounds.xMax + i * 4);
    
            __m128 bymax = _mm_load_ps(trunkB.subNodeBounds.yMax + i * 4);
    
            __m128 bzmax = _mm_load_ps(trunkB.subNodeBounds.zMax + i * 4);


            rx0 = _mm_cmp_ps(axmax, bxmin, _CMP_GE_OQ);

            ry0 = _mm_cmp_ps(aymax, bymin, _CMP_GE_OQ);

            rz0 = _mm_cmp_ps(azmax, bzmin, _CMP_GE_OQ);

            rx1 = _mm_cmp_ps(axmin, bxmax, _CMP_LE_OQ);

            ry1 = _mm_cmp_ps(aymin, bymax, _CMP_LE_OQ);

            rz1 = _mm_cmp_ps(azmin, bzmax, _CMP_LE_OQ);


            __m128 resultBool = _mm_and_ps(  _mm_and_ps(_mm_and_ps(rx0, ry0), rz0),
                                                _mm_and_ps( _mm_and_ps(rx1, ry1), rz1)  );


            _mm_store_ps(tempBuff, resultBool);

			
            result[a][0 + i * 4] =  tempBuff[0];
            result[a][1 + i * 4] =  tempBuff[1];
            result[a][2 + i * 4] =  tempBuff[2];
            result[a][3 + i * 4] =  tempBuff[3];

        }
	}
	return result;
}






}