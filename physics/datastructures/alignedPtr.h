#pragma once

#include <utility>
#include <cstddef>

void deleteAligned(void* buf);
void* createAligned(std::size_t size, std::size_t align);

template<typename T>
class UniqueAlignedPointer {
	T* data;

public:
	UniqueAlignedPointer() : data(nullptr) {}
	UniqueAlignedPointer(std::size_t size, std::size_t align = alignof(T)) : 
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
		std::swap(this->data, other.data);

		return *this;
	}
};

template<typename T>
class SharedAlignedPointer {
	T* data;
	std::size_t* refCount;

public:
	SharedAlignedPointer() : data(nullptr), refCount(nullptr) {}
	SharedAlignedPointer(std::size_t size, std::size_t align = alignof(T)) :
		data(static_cast<T*>(createAligned(sizeof(T)* size, align))),
		refCount(new std::size_t(1)) {}
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

