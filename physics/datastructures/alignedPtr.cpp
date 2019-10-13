#include "alignedPtr.h"

#include <stdlib.h>
#include <exception>

void deleteAligned(void* buf) {
	_aligned_free(buf);
}

void* createAligned(size_t size, size_t align) {
	void* buf = _aligned_malloc(size, align);
	if (!buf) {
		throw std::bad_alloc();
	}

	if ((reinterpret_cast<size_t>(buf) % align) != 0) {
		_aligned_free(buf);
		throw "Error, did not align buffer storage!";
	}
	return buf;
}
