#pragma once

template<typename T>
class SharedArrayPtr {
	T* ptr;
	size_t* refCount;
	inline SharedArrayPtr(T* data, size_t* refCount) : ptr(data), refCount(refCount) {}
public:

	inline SharedArrayPtr() : ptr(nullptr), refCount(nullptr) {};
	explicit inline SharedArrayPtr(T* data) : ptr(data), refCount((data == nullptr) ? nullptr : new size_t(0)) {};

	inline SharedArrayPtr(const SharedArrayPtr<T>& other) : ptr(other.ptr), refCount(other.refCount) {
		if(refCount != nullptr) ++(*refCount);
	}

	inline const SharedArrayPtr<T>& operator=(const SharedArrayPtr<T>& other) {
		this->~SharedArrayPtr();
		this->ptr = other.ptr;
		this->refCount = other.refCount;
		if (refCount != nullptr) ++(*refCount);
		return *this;
	}

	inline SharedArrayPtr(SharedArrayPtr<T>&& other) : ptr(other.ptr), refCount(other.refCount) {
		other.ptr = nullptr;
		other.refCount = nullptr;
	}

	inline SharedArrayPtr& operator=(SharedArrayPtr<T>&& other) {
		this->~SharedArrayPtr();
		this->ptr = other.ptr;
		this->refCount = other.refCount;
		other.ptr = nullptr;
		other.refCount = nullptr;
		return *this;
	}

	inline static SharedArrayPtr<T> staticSharedArrayPtr(T* data) { return SharedArrayPtr<T>(data, nullptr); }

	inline ~SharedArrayPtr() {
		if (refCount != nullptr) {
			if (*refCount == 0) {
				delete refCount;
				delete[] ptr;
			} else {
				--(*refCount);
			}
		}
	}
	inline T& operator*() const { return *ptr; }
	inline T& operator[](size_t index) const { return ptr[index]; }
	inline T* operator+(size_t offset) const { return ptr + offset; }
	inline T* operator-(size_t offset) const { return ptr - offset; }
	inline T* operator->() const { return ptr; }
	inline T* get() const { return ptr; }
	inline bool operator==(T* other) const { return ptr == other; }
	inline bool operator!=(T* other) const { return ptr != other; }
	inline bool operator<=(T* other) const { return ptr <= other; }
	inline bool operator>=(T* other) const { return ptr >= other; }
	inline bool operator<(T* other) const { return ptr < other; }
	inline bool operator>(T* other) const { return ptr > other; }
};

