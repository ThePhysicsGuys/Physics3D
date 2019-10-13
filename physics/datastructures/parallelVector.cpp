#include "parallelVector.h"

#include <new>
#include <cstdlib>

void* createParallelVecBufWithBlocks(size_t blockCount) {
	void* vecs = _aligned_malloc(sizeof(ParallelVec3) * blockCount, alignof(ParallelVec3));
	if (!vecs) {
		throw std::bad_alloc();
	}

	if ((reinterpret_cast<size_t>(vecs) & 0b00011111) != 0) {
		_aligned_free(vecs);
		throw "Error, did not align ParallelVec3 storage!";
	}
	return vecs;
}

inline void deleteParallelVecBuf(void* buf) {
	_aligned_free(buf);
}

void* ParallelVec3::operator new(size_t count) {
	return createParallelVecBufWithBlocks(count);
}
void* ParallelVec3::operator new[](size_t count) {
	return createParallelVecBufWithBlocks(count);
}

void ParallelVec3::operator delete(void* buf) {
	deleteParallelVecBuf(buf);
}
void ParallelVec3::operator delete[](void* buf) {
	deleteParallelVecBuf(buf);
}
