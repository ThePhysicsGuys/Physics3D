#include "testsMain.h"

#include "../util/log.h"
#include "../physics/math/cframe.h"
#include "../physics/datastructures/buffers.h"
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
#define ITER_MAX 10
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

//! Deprecated
/*
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
