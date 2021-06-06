#include "aligned_alloc.h"

#include <malloc.h>
#include <stdlib.h>

namespace P3D {
void* aligned_malloc(size_t size, size_t align) {
#ifdef _MSC_VER
	return _aligned_malloc(size, align);
#else
	return aligned_alloc(align, size);
#endif
}
void aligned_free(void* ptr) {
#ifdef _MSC_VER
	_aligned_free(ptr);
#else
	free(ptr);
#endif
}
};