#pragma once

#include "../math/linalg/vec.h"
#include "../../util/log.h"

struct alignas(32) ParallelVec3 {
	float xvalues[8];
	float yvalues[8];
	float zvalues[8];

	ParallelVec3() = default;
	inline ParallelVec3(const Vec3f data[8]) {
		this->load(data);
	}

	inline void load(const Vec3f data[8]) {
		for (int i = 0; i < 8; i++) {
			xvalues[i] = data[i].x;
			yvalues[i] = data[i].y;
			zvalues[i] = data[i].z;
		}
	}

	inline Vec3f operator[](unsigned int i) const {
		return Vec3f(xvalues[i], yvalues[i], zvalues[i]);
	}
	
	inline void setVec(unsigned int i, float x, float y, float z) {
		xvalues[i] = x;
		yvalues[i] = y;
		zvalues[i] = z;
	}
	inline void setVec(unsigned int i, Vec3f v) {
		xvalues[i] = v.x;
		yvalues[i] = v.y;
		zvalues[i] = v.z;
	}

	static void* operator new(size_t count);
	static void* operator new[](size_t count);


	static void operator delete(void* obj);
	static void operator delete[](void* obj);
};

inline ParallelVec3* createParallelVecBuf(size_t size) {
	size_t blockCount = (size + 7) / 8;
	ParallelVec3* vecs = new ParallelVec3[blockCount];

	if ((reinterpret_cast<size_t>(vecs) & 0b00011111) != 0) {
		Log::error("VecBuf Not aligned!");
	}
	return vecs;
}

// fill last unused entries of the last block with the last vector to avoid issues with SIMD
inline void fixFinalBlock(ParallelVec3* buf, size_t size, Vec3f value) {
	size_t alignedSize = size & 0xFFFFFFFFFFFFFFF8;
	size_t remaining = size & 7;
	size_t lastBlockIndex = size / 8;

	if (remaining != 0) {
		for (int i = static_cast<int>(remaining); i < 8; i++) {
			buf[lastBlockIndex].setVec(i, value);
		}
	}
}

inline void fixFinalBlock(ParallelVec3* buf, size_t size) {
	fixFinalBlock(buf, size, buf[(size-1) >> 3][(size-1) & 0x7]);
}

inline ParallelVec3* createAndFillParallelVecBuf(const Vec3f* data, size_t size) {
	ParallelVec3* vecs = createParallelVecBuf(size);

	// fill all the easly fillable blocks with their data
	for (size_t i = 0; i < size / 8; i++) {
		vecs[i].load(data + i * 8);
	}

	size_t alignedSize = size & 0xFFFFFFFFFFFFFFF8;

	// fill the remainder of the last block with the last vector, just to have filler data there
	// this makes it that for example getFurthestInDirection does not have to include a special case for the last block of vecs
	size_t remaining = size & 7;
	if (remaining != 0) { // not perfectly aligned to block count
		Vec3f lastBlock[8];
		Vec3f lastVec = data[alignedSize + remaining - 1];
		size_t lastBlockIndex = size / 8;
		for (size_t i = 0; i < remaining; i++)
			lastBlock[i] = data[alignedSize + i];

		for (size_t i = remaining; i < 8; i++)
			lastBlock[i] = lastVec;

		vecs[lastBlockIndex].load(lastBlock);
	}
	return vecs;
}
