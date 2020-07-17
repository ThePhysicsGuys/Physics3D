
#include <cstddef>
#include <utility>
#include <assert.h>

/*
	Warning: This is an unmanaged object, does not automatically free memory on delete
*/
template<typename T>
class UnmanagedArray {
	T* ptr;
	T* endPtr;
public:
	// ignore endPtr, endPtr is only valid if ptr is not nullptr
	UnmanagedArray() : ptr(nullptr) {}
	UnmanagedArray(T* ptr, std::size_t size) : ptr(ptr), endPtr(ptr + size) {}

	T& operator[](std::size_t index) {
		assert(ptr != nullptr);
		assert(index >= 0 && ptr + index < endPtr);
		return ptr[index];
	}
	const T& operator[](std::size_t index) const {
		assert(ptr != nullptr);
		assert(index >= 0 && ptr + index < endPtr);
		return ptr[index];
	}
	T& operator*() {assert(ptr != nullptr); return *ptr;}
	const T& operator*() const {assert(ptr != nullptr); return *ptr;}

	T* begin() {assert(ptr != nullptr); return ptr;}
	const T* begin() const {assert(ptr != nullptr); return ptr;}
	T* end() {assert(ptr != nullptr); return endPtr;}
	const T* end() const {assert(ptr != nullptr); return endPtr;}

	T* get() {return ptr;}
	const T* get() const {return ptr;}

	T* getEnd() {assert(ptr != nullptr); return endPtr;}
	const T* getEnd() const {assert(ptr != nullptr); return endPtr;}

	T* getPtrToFree() {
		return ptr;
	}
};
