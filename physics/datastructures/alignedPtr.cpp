#include "alignedPtr.h"

#include <stdlib.h>
#include <exception>

void deleteAligned(void* buf) {
#ifdef _MSC_VER
	_aligned_free(buf);
#else
	free(buf);
#endif
}

void* createAligned(size_t size, size_t align) {
#ifdef _MSC_VER
	void* buf = _aligned_malloc(size, align);
#else
	void* buf = aligned_alloc(align, size);
#endif
	if (!buf) {
		throw std::bad_alloc();
	}

	if ((reinterpret_cast<size_t>(buf) % align) != 0) {
		deleteAligned(buf);
		throw "Error, did not align buffer storage!";
	}
	return buf;
}
