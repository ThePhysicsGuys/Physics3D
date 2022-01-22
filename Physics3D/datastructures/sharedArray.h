#pragma once

namespace P3D {
template <typename T>
class SharedArrayPtr {
	T* ptr;
	size_t* refCount;

	inline SharedArrayPtr(T* data, size_t* refCount)
		: ptr(data)
		, refCount(refCount) {}

public:
	SharedArrayPtr()
		: ptr(nullptr)
		, refCount(nullptr) {}

	explicit SharedArrayPtr(T* data)
		: ptr(data)
		, refCount((data == nullptr) ? nullptr : new size_t(0)) {}

	SharedArrayPtr(const SharedArrayPtr<T>& other)
		: ptr(other.ptr)
		, refCount(other.refCount) {
		if (refCount != nullptr)
			++(*refCount);
	}

	SharedArrayPtr<T>& operator=(const SharedArrayPtr<T>& other) {
		this->~SharedArrayPtr();
		this->ptr = other.ptr;
		this->refCount = other.refCount;
		if (refCount != nullptr)
			++(*refCount);

		return *this;
	}

	SharedArrayPtr(SharedArrayPtr<T>&& other) noexcept
		: ptr(other.ptr)
		, refCount(other.refCount) {
		other.ptr = nullptr;
		other.refCount = nullptr;
	}

	SharedArrayPtr& operator=(SharedArrayPtr<T>&& other) noexcept {
		this->~SharedArrayPtr();
		this->ptr = other.ptr;
		this->refCount = other.refCount;
		other.ptr = nullptr;
		other.refCount = nullptr;
		return *this;
	}

	static SharedArrayPtr<T> staticSharedArrayPtr(T* data) {
		return SharedArrayPtr<T>(data, nullptr);
	}

	~SharedArrayPtr() {
		if (refCount != nullptr) {
			if (*refCount == 0) {
				delete refCount;
				delete[] ptr;
			} else {
				--(*refCount);
			}
		}
	}

	T& operator*() const {
		return *ptr;
	}

	T& operator[](size_t index) const {
		return ptr[index];
	}

	T* operator+(size_t offset) const {
		return ptr + offset;
	}

	T* operator-(size_t offset) const {
		return ptr - offset;
	}

	T* operator->() const {
		return ptr;
	}

	T* get() const {
		return ptr;
	}

	bool operator==(T* other) const {
		return ptr == other;
	}

	bool operator!=(T* other) const {
		return ptr != other;
	}

	bool operator<=(T* other) const {
		return ptr <= other;
	}

	bool operator>=(T* other) const {
		return ptr >= other;
	}

	bool operator<(T* other) const {
		return ptr < other;
	}

	bool operator>(T* other) const {
		return ptr > other;
	}
};
};
