#include "testsMain.h"

#include "../engine/math/vec3.h"
#include "../engine/datastructures/parallelVector.h"
#include <immintrin.h>
#include "../util/log.h"

Vec3f vecs[]{{0,1,2},{1,4,3},{7,5,9},{1,2,4},
             {7,9,6},{0.1f, 0.3f, -2.3f},{1,8,6}};
ParallelVec3 parallelV(vecs);

TEST_CASE(nonParallelDot) {
	float curDot[8] = {0,0,0,0,0,0,0,0};

	for(int iter = 0; iter < 1000; iter++)
	for(float x = 0; x < 1.5; x += 0.014) {
		for(float y = 0; y < 1.5; y += 0.014) {
			for(float z = 0; z < 1.5; z += 0.014) {
				Vec3f vec(x, y, z);
				//Vec3f vec(0.3, 0.4, 0.5);

				for(int i = 0; i < 8; i++) {
					curDot[i] += vecs[i] * vec;
				}
			}
		}
	}

	float sum = 0;
	for(int i = 0; i < 8; i++) {
		sum += curDot[i];
	}

	Log::debug("%f", sum);
}
TEST_CASE(customParallelDot) {
	__m256 curDot = _mm256_set1_ps(0);

	for(int iter = 0; iter < 1000; iter++)
	for(float x = 0; x < 1.5; x += 0.014) {
		for(float y = 0; y < 1.5; y += 0.014) {
			for(float z = 0; z < 1.5; z += 0.014) {
				Vec3f vec(x, y, z);
				//Vec3f vec(0.3, 0.4, 0.5);
				__m256 localDot = parallelV.dot(vec);
				curDot = _mm256_add_ps(curDot, localDot);
			}
		}
	}

	union Converter {
		__m256 in;
		float out[8];
	};

	Converter t;
	t.in = curDot;

	float sum = 0;
	for(int i = 0; i < 8; i++) {
		sum += t.out[i];
	}
	Log::debug("%f", sum);
}
TEST_CASE(generatedParallelDot) {
	float curDot[8] = {0,0,0,0,0,0,0,0};

	for(int iter = 0; iter < 1000; iter++)
		for(float x = 0; x < 1.5; x += 0.014) {
			for(float y = 0; y < 1.5; y += 0.014) {
				for(float z = 0; z < 1.5; z += 0.014) {
					Vec3f vec(x, y, z);
					//Vec3f vec(0.3, 0.4, 0.5);
					for(int i = 0; i < 8; i++) {
						curDot[i] += parallelV.x[i] * vec.x + parallelV.y[i] * vec.y + parallelV.z[i] * vec.z;
					}
				}
			}
		}

	float sum = 0;
	for(int i = 0; i < 8; i++) {
		sum += curDot[i];
	}

	Log::debug("%f", sum);
}