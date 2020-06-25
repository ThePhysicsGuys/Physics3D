#include "benchmark.h"

#include "../physics/math/rotation.h"
#include <random>

static double randomDouble() {
	return double(rand()) / RAND_MAX;
}

#define ROTATION_BENCH_SIZE 10000000

class RotationLocalToGlobalVecCPU : public Benchmark {
	Rotation rot;
	Vec3 curValue;
public:
	RotationLocalToGlobalVecCPU() : Benchmark("rotationL2GVecCPU") {}

	void init() override {
		this->rot = Rotation::fromEulerAngles(randomDouble(), randomDouble(), randomDouble());
		curValue = Vec3(randomDouble(), randomDouble(), randomDouble());
	}
	void run() override {
		for(int round = 0; round < ROTATION_BENCH_SIZE * 100; round++) {
			curValue = rot.localToGlobal(curValue);
		}
	}
} rotationLocalToGlobalVecCPU;

class RotationLocalToGlobalMatCPU : public Benchmark {
	Rotation rot;
	SymmetricMat3 curValue;
public:
	RotationLocalToGlobalMatCPU() : Benchmark("rotationL2GMatCPU") {}

	void init() override {
		this->rot = Rotation::fromEulerAngles(randomDouble(), randomDouble(), randomDouble());
		curValue = SymmetricMat3{randomDouble(), randomDouble(), randomDouble(), randomDouble(), randomDouble(), randomDouble()};
	}
	void run() override {
		for(int round = 0; round < ROTATION_BENCH_SIZE * 100; round++) {
			curValue = rot.localToGlobal(curValue);
		}
	}
} rotationLocalToGlobalMatCPU;

class RotationLocalToGlobalRotCPU : public Benchmark {
	Rotation rot;
	Rotation curValue;
public:
	RotationLocalToGlobalRotCPU() : Benchmark("rotationL2GRotCPU") {}

	void init() override {
		this->rot = Rotation::fromEulerAngles(randomDouble(), randomDouble(), randomDouble());
		curValue = Rotation::fromEulerAngles(randomDouble(), randomDouble(), randomDouble());
	}
	void run() override {
		for(int round = 0; round < ROTATION_BENCH_SIZE * 100; round++) {
			curValue = rot.localToGlobal(curValue);
		}
	}
} rotationLocalToGlobalRotCPU;



class RotationLocalToGlobalVecMem : public Benchmark {
	Rotation* rot = nullptr;
	Vec3 curValue;
public:
	RotationLocalToGlobalVecMem() : Benchmark("rotationL2GVecMem") {}

	void init() override {
		if(rot == nullptr) this->rot = new Rotation[ROTATION_BENCH_SIZE];
		for(int round = 0; round < ROTATION_BENCH_SIZE; round++){
			this->rot[round] = Rotation::fromEulerAngles(randomDouble(), randomDouble(), randomDouble());
		}
		curValue = Vec3(randomDouble(), randomDouble(), randomDouble());
	}
	~RotationLocalToGlobalVecMem() {
		delete[] rot;
	}
	void run() override {
		for(int round = 0; round < ROTATION_BENCH_SIZE; round++) {
			curValue = rot[round].localToGlobal(curValue);
		}
	}
} rotationLocalToGlobalVecMem;

class RotationLocalToGlobalMatMem : public Benchmark {
	Rotation* rot = nullptr;
	SymmetricMat3 curValue;
public:
	RotationLocalToGlobalMatMem() : Benchmark("rotationL2GMatMem") {}

	void init() override {
		if(rot == nullptr) this->rot = new Rotation[ROTATION_BENCH_SIZE];
		for(int round = 0; round < ROTATION_BENCH_SIZE; round++){
			this->rot[round] = Rotation::fromEulerAngles(randomDouble(), randomDouble(), randomDouble());
		}
		curValue = SymmetricMat3{randomDouble(), randomDouble(), randomDouble(), randomDouble(), randomDouble(), randomDouble()};
	}
	~RotationLocalToGlobalMatMem() {
		delete[] rot;
	}
	void run() override {
		for(int round = 0; round < ROTATION_BENCH_SIZE; round++) {
			curValue = rot[round].localToGlobal(curValue);
		}
	}
} rotationLocalToGlobalMatMem;

class RotationLocalToGlobalRotMem : public Benchmark {
	Rotation* rot = nullptr;
	Rotation curValue;
public:
	RotationLocalToGlobalRotMem() : Benchmark("rotationL2GRotMem") {}

	void init() override {
		if(rot == nullptr) this->rot = new Rotation[ROTATION_BENCH_SIZE];
		for(int round = 0; round < ROTATION_BENCH_SIZE; round++){
			this->rot[round] = Rotation::fromEulerAngles(randomDouble(), randomDouble(), randomDouble());
		}
		curValue = Rotation::fromEulerAngles(randomDouble(), randomDouble(), randomDouble());
	}
	~RotationLocalToGlobalRotMem() {
		delete[] rot;
	}
	void run() override {
		for(int round = 0; round < ROTATION_BENCH_SIZE; round++) {
			curValue = rot[round].localToGlobal(curValue);
		}
	}
} rotationLocalToGlobalRotMem;
