#include "boundsTree.h" 

#include "../datastructures/alignedPtr.h"
#include <immintrin.h>

namespace P3D {

OverlapMatrix TrunkSIMDHelperFallback::computeBoundsOverlapMatrixAVX(const TreeTrunk& trunkA, int trunkASize, const TreeTrunk& trunkB, int trunkBSize) {
	OverlapMatrix result;

    __m256 rx0 = _mm256_set1_ps(0);
    __m256 ry0 = _mm256_set1_ps(0);
    __m256 rz0 = _mm256_set1_ps(0);

    __m256 rx1 = _mm256_set1_ps(0);
    __m256 ry1 = _mm256_set1_ps(0);
    __m256 rz1 = _mm256_set1_ps(0);
    
    alignas(32) float tempBuff[8];


    __m256 bxmin = _mm256_load_ps(trunkB.subNodeBounds.xMin);
    
    __m256 bymin = _mm256_load_ps(trunkB.subNodeBounds.yMin);
    
    __m256 bzmin = _mm256_load_ps(trunkB.subNodeBounds.zMin);
    
    __m256 bxmax = _mm256_load_ps(trunkB.subNodeBounds.xMax);
    
    __m256 bymax = _mm256_load_ps(trunkB.subNodeBounds.yMax);
    
    __m256 bzmax = _mm256_load_ps(trunkB.subNodeBounds.zMax);



	for(int a = 0; a < trunkASize; a++) {
		
    	BoundsTemplate<float> aBounds0 = trunkA.getBoundsOfSubNode(a);
     
        __m256 axmin = _mm256_set1_ps(aBounds0.min.x);

        __m256 aymin = _mm256_set1_ps(aBounds0.min.y);

        __m256 azmin = _mm256_set1_ps(aBounds0.min.z);

        __m256 axmax = _mm256_set1_ps(aBounds0.max.x);

        __m256 aymax = _mm256_set1_ps(aBounds0.max.y);

        __m256 azmax = _mm256_set1_ps(aBounds0.max.z);



            rx0 = _mm256_cmp_ps(axmax, bxmin, _CMP_GE_OQ);

            ry0 = _mm256_cmp_ps(aymax, bymin, _CMP_GE_OQ);

            rz0 = _mm256_cmp_ps(azmax, bzmin, _CMP_GE_OQ);

            rx1 = _mm256_cmp_ps(axmin, bxmax, _CMP_LE_OQ);

            ry1 = _mm256_cmp_ps(aymin, bymax, _CMP_LE_OQ);

            rz1 = _mm256_cmp_ps(azmin, bzmax, _CMP_LE_OQ);


            __m256 resultBool = _mm256_and_ps(  _mm256_and_ps(_mm256_and_ps(rx0, ry0), rz0),
                                                _mm256_and_ps( _mm256_and_ps(rx1, ry1), rz1)  );


            _mm256_store_ps(tempBuff, resultBool);

			
            result[a][0] =  tempBuff[0];
            result[a][1] =  tempBuff[1];
            result[a][2] =  tempBuff[2];
            result[a][3] =  tempBuff[3];
            result[a][4] =  tempBuff[4];
            result[a][5] =  tempBuff[5];
            result[a][6] =  tempBuff[6];
            result[a][7] =  tempBuff[7];

		
	}
	return result;
}






}