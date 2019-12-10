#pragma once

#include "../math/linalg/vec.h"
#include "../datastructures/alignedPtr.h"

struct Triangle;

inline size_t getOffset(size_t size) {
	return (size + 7) & 0xFFFFFFFFFFFFFFF8;
}
inline UniqueAlignedPointer<float> createParallelVecBuf(size_t size) {
	return UniqueAlignedPointer<float>(getOffset(size) * 3, 32);
}
inline UniqueAlignedPointer<int> createParallelTriangleBuf(size_t size) {
	return UniqueAlignedPointer<int>(getOffset(size) * 3, 32);
}
UniqueAlignedPointer<float> createAndFillParallelVecBuf(size_t size, const Vec3f* vectors);
UniqueAlignedPointer<int> createAndFillParallelTriangleBuf(size_t size, const Triangle* triangles);

void setInBuf(float* buf, size_t size, size_t index, const Vec3f& value);

template<typename T>
UniqueAlignedPointer<T> copy(const UniqueAlignedPointer<T>& buf, size_t size) {
	size_t totalBufSize = getOffset(size) * 3;
	UniqueAlignedPointer<T> result(totalBufSize, 32);

	for(size_t i = 0; i < totalBufSize; i++) {
		result[i] = buf[i];
	}

	return result;
}

template<typename T>
void fixFinalBlock(T* buf, size_t size) {
	size_t offset = getOffset(size);
	T* xValues = buf;
	T* yValues = buf + offset;
	T* zValues = buf + 2 * offset;

	for(size_t i = size; i < offset; i++) {
		xValues[i] = xValues[size - 1];
		yValues[i] = yValues[size - 1];
		zValues[i] = zValues[size - 1];
	}
}


