#include <cstddef>
#include <utility>
#include <assert.h>

namespace P3D {
template<typename T>
struct DefaultArrayDelete {
	void operator()(T* arr) const {
		delete[] arr;
	}
};
struct DoNothingDelete {
	void operator()(void* arr) const noexcept {}
};

template<typename T, typename Deleter = DefaultArrayDelete<T>>
class UniqueArrayPtr {
	T* ptr;
	T* endPtr;

	Deleter deleter;
public:
	// ignore endPtr, endPtr is only valid if ptr is not nullptr
	UniqueArrayPtr() : ptr(nullptr) {}
	UniqueArrayPtr(T* ptr, std::size_t size, Deleter deleter = {}) : ptr(ptr), endPtr(ptr + size), deleter(deleter) {}
	~UniqueArrayPtr() {
		deleter(this->ptr);
	}
	UniqueArrayPtr(const UniqueArrayPtr&) = delete;
	UniqueArrayPtr& operator=(const UniqueArrayPtr&) = delete;

	UniqueArrayPtr(UniqueArrayPtr&& other) noexcept : ptr(other.ptr), endPtr(other.endPtr), deleter(std::move(other.deleter)) {
		other.ptr = nullptr;
	}
	UniqueArrayPtr& operator=(UniqueArrayPtr&& other) noexcept {
		std::swap(this->ptr, other.ptr);
		std::swap(this->endPtr, other.endPtr);
		std::swap(this->deleter, other.deleter);
		return *this;
	}

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
	T& operator*() { assert(ptr != nullptr); return *ptr; }
	const T& operator*() const { assert(ptr != nullptr); return *ptr; }

	T* begin() { assert(ptr != nullptr); return ptr; }
	const T* begin() const { assert(ptr != nullptr); return ptr; }
	T* end() { assert(ptr != nullptr); return endPtr; }
	const T* end() const { assert(ptr != nullptr); return endPtr; }

	T* get() { return ptr; }
	const T* get() const { return ptr; }

	T* getEnd() { assert(ptr != nullptr); return endPtr; }
	const T* getEnd() const { assert(ptr != nullptr); return endPtr; }
};
};