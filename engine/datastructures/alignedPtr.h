#pragma once

#include <utility>

void deleteAligned(void* buf);
void* createAligned(size_t size, size_t align);

template<typename T>
class UniqueAlignedPointer {
	T* data;

public:
	UniqueAlignedPointer() : data(nullptr) {}
	UniqueAlignedPointer(size_t size, size_t align = alignof(T)) : 
		data(static_cast<T*>(createAligned(sizeof(T)* size, align))) {}
	~UniqueAlignedPointer() {
		deleteAligned(static_cast<void*>(data));
	}

	inline T* get() const { return data; }
	operator T*() const { return data; }

	UniqueAlignedPointer(const UniqueAlignedPointer& other) = delete;
	UniqueAlignedPointer& operator=(const UniqueAlignedPointer& other) = delete;

	UniqueAlignedPointer(UniqueAlignedPointer&& other) noexcept : data(other.data) {
		other.data = nullptr;
	}
	UniqueAlignedPointer& operator=(UniqueAlignedPointer&& other) noexcept {
		this->data = other.data;
		other.data = nullptr;

		return *this;
	}
};

template<typename T>
class SharedAlignedPointer {
	T* data;
	size_t* refCount;

public:
	SharedAlignedPointer() : data(nullptr), refCount(nullptr) {}
	SharedAlignedPointer(size_t size, size_t align = alignof(T)) :
		data(static_cast<T*>(createAligned(sizeof(T)* size, align))),
		refCount(new size_t(1)) {}
	~SharedAlignedPointer() {
		if (refCount != nullptr && --(*refCount) == 0) {
			deleteAligned(static_cast<void*>(data));
			delete refCount;
		}
	}

	inline T* get() const { return data; }
	operator T* () const { return data; }

	SharedAlignedPointer(const SharedAlignedPointer& other) : data(other.data), refCount(other.refCount) {
		(*refCount)++;
	}
	SharedAlignedPointer& operator=(const SharedAlignedPointer& other) {
		this->~SharedAlignedPointer();

		this->data = other.data;
		this->refCount = other.refCount;

		(*refCount)++;

		return *this;
	}

	SharedAlignedPointer(SharedAlignedPointer&& other) noexcept : data(other.data), refCount(other.refCount) {
		other.data = nullptr;
		other.refCount = nullptr;
	}
	SharedAlignedPointer& operator=(SharedAlignedPointer&& other) noexcept {
		std::swap(this->data, other.data);
		std::swap(this->refCount, other.refCount);

		return *this;
	}
};