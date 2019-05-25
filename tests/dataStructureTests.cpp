#include "testsMain.h"

#include "../util/log.h"
#include "../engine/math/cframe.h"
#include "../engine/smallVector.h"
#include "../application/buffers.h"
#include <vector>

volatile double t;

/*TEST_CASE(profileVector) {
	std::vector<CFrame> vector(10);

	CFrame f(Vec3(1, 8, 3), Mat3(1, 4, 8, 9, 6, 5, 7, 4, 8));

	double total = 0.0;

	for(int iter = 0; iter < 2000; iter++) {
		for(int i = 0; i < 100000; i++) {
			vector.push_back(f);
		}
		for(int i = 0; i < 100000; i++) {
			total += vector.at(i).position.x;
		}
		vector.clear();
	}

	t = total;
}

TEST_CASE(profileAddableBuffer) {
	AddableBuffer<CFrame> vector(10);

	CFrame f(Vec3(1, 8, 3), Mat3(1, 4, 8, 9, 6, 5, 7, 4, 8));

	double total = 0.0;

	for(int iter = 0; iter < 2000; iter++) {
		for(int i = 0; i < 100000; i++) {
			vector.add(f);
		}
		for(int i = 0; i < 100000; i++) {
			total += vector.data[i].position.x;
		}
		vector.clear();
	}

	t = total;
}*/

#define VECTOR_COUNT 200
#define ITER_MAX 1000
#define VECTOR_SIZE 3

TEST_CASE(profileVectorOfVectors) {
	AddableBuffer<int>* vector = new AddableBuffer<int>[VECTOR_COUNT];
	//Log::debug("sizeof(vector) == %d", sizeof(AddableBuffer<int>));
	//Log::debug("sizeof(std::vector) == %d", sizeof(std::vector<int>));
	for(int i = 0; i < VECTOR_COUNT; i++) {
		for(int j = 0; j < VECTOR_SIZE; j++) {
			vector[i].data[j] = (i + j) % 3469;
		}
	}

	unsigned int sum = 0;

	for(int iter = 0; iter < ITER_MAX; iter++) {
		for(int i = 0; i < VECTOR_COUNT; i++) {
			for(int j = 0; j < VECTOR_SIZE; j++) {
				sum += vector[i].data[j];
			}
		}
	}

	Log::debug("Total %d", sum);
}

TEST_CASE(profileSmallVectorOfVectorsTooLarge) {
	SmallVector<int, 5>* vector = new SmallVector<int, 5>[VECTOR_COUNT];
	//Log::debug("sizeof(vector) == %d", sizeof(SmallVector<int, 5>));
	for(int i = 0; i < VECTOR_COUNT; i++) {
		int newData[VECTOR_SIZE];
		for(int j = 0; j < VECTOR_SIZE; j++) {
			newData[j] = (i + j) % 3469;
		}
		vector[i].setData(newData, VECTOR_SIZE);
	}

	unsigned int sum = 0;

	for(int iter = 0; iter < ITER_MAX; iter++) {
		for(int i = 0; i < VECTOR_COUNT; i++) {
			for(int j = 0; j < VECTOR_SIZE; j++) {
				sum += vector[i][j];
			}
		}
	}

	Log::debug("Total %d", sum);
}
TEST_CASE(profileSmallVectorOfVectorsJustRight) {
	SmallVector<int, 3>* vector = new SmallVector<int, 3>[VECTOR_COUNT];
	//Log::debug("sizeof(vector) == %d", sizeof(SmallVector<int, 3>));
	//Log::debug("offset(size) == %d", offsetof(SmallVector<int, 5>, size));
	for(int i = 0; i < VECTOR_COUNT; i++) {
		int newData[VECTOR_SIZE];
		for(int j = 0; j < VECTOR_SIZE; j++) {
			newData[j] = (i + j) % 3469;
		}
		vector[i].setData(newData, VECTOR_SIZE);
	}

	unsigned int sum = 0;

	for(int iter = 0; iter < ITER_MAX; iter++) {
		for(int i = 0; i < VECTOR_COUNT; i++) {
			for(int j = 0; j < VECTOR_SIZE; j++) {
				sum += vector[i][j];
			}
		}
	}

	Log::debug("Total %d", sum);
}
/*TEST_CASE(profileSmallVectorOfVectorsTooSmall) {
	SmallVector<int, 2>* vector = new SmallVector<int, 2>[VECTOR_COUNT];
	Log::debug("sizeof(vector) == %d", sizeof(SmallVector<int, 5>));
	for(int i = 0; i < VECTOR_COUNT; i++) {
		int newData[VECTOR_SIZE];
		for(int j = 0; j < VECTOR_SIZE; j++) {
			newData[j] = (i + j) % 3469;
		}
		vector[i].setData(newData, VECTOR_SIZE);
	}

	unsigned int sum = 0;

	for(int iter = 0; iter < ITER_MAX; iter++) {
		for(int i = 0; i < VECTOR_COUNT; i++) {
			for(int j = 0; j < VECTOR_SIZE; j++) {
				sum += vector[i][j];
			}
		}
	}

	Log::debug("Total %d", sum);
}*/

TEST_CASE(unpackNShit) {
	__m256 a = _mm256_set_ps(1,2,3,4,5,6,7,8);
	__m256 b = _mm256_set_ps(10,20,30,40,50,60,70,80);
	//__m256 a = _mm256_set_ps(1,1,1,1,2,2,2,2);
	//__m256 b = _mm256_set_ps(3,3,3,3,4,4,4,4);

	//__m256 ta = _mm256_permute2f128_ps(a, a, 1);
	//__m256 tb = _mm256_permute2f128_ps(b, b, 1);

	__m256 xzxzydyd = _mm256_unpackhi_ps(a, b);
	__m256 XZXZYDYD = _mm256_unpacklo_ps(a, b);

	__m256 xzxzXZXZ = _mm256_permute2f128_ps(xzxzydyd, XZXZYDYD, 0b00000010);
	__m256 YDYDydyd = _mm256_permute2f128_ps(xzxzydyd, XZXZYDYD, 0b00010011);

	__m256 e = _mm256_castpd_ps(_mm256_unpackhi_pd(_mm256_castps_pd(xzxzXZXZ), _mm256_castps_pd(YDYDydyd)));
	__m256 f = _mm256_castpd_ps(_mm256_unpacklo_pd(_mm256_castps_pd(xzxzXZXZ), _mm256_castps_pd(YDYDydyd)));

	/*__m256 c = _mm256_castpd_ps(_mm256_unpackhi_pd(_mm256_castps_pd(a), _mm256_castps_pd(a)));
	__m256 c2 = _mm256_castpd_ps(_mm256_unpacklo_pd(_mm256_castps_pd(a), _mm256_castps_pd(a)));
	__m256 d = _mm256_castpd_ps(_mm256_unpacklo_pd(_mm256_castps_pd(b), _mm256_castps_pd(b)));
	__m256 d2 = _mm256_castpd_ps(_mm256_unpackhi_pd(_mm256_castps_pd(b), _mm256_castps_pd(b)));*/

	/*__m256 e = _mm256_blend_ps(c, d, 0b00110011);
	__m256 f = _mm256_blend_ps(c, d, 0b11001100);*/

	/*__m256 c = _mm256_unpackhi_ps(a, b);
	__m256 d = _mm256_unpacklo_ps(a, b);

	d = _mm256_permute2f128_ps(d, d, 1);



	__m256 e = _mm256_castpd_ps(_mm256_unpackhi_pd(_mm256_castps_pd(c), _mm256_castps_pd(d)));
	__m256 f = _mm256_castpd_ps(_mm256_unpacklo_pd(_mm256_castps_pd(d), _mm256_castps_pd(c)));*/
}