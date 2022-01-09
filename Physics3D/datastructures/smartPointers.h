#pragma once

#include <memory>
#include <atomic>

namespace P3D {

struct RC {
	std::size_t refCount = 0;

	virtual ~RC() {}
};

struct AtomicRC {
	std::atomic<std::size_t> refCount = 0;

	virtual ~AtomicRC() {}
};

template<typename T>
void intrusive_ptr_add_ref(T* iptr) noexcept {
	iptr->refCount++;
}

template<typename T>
void intrusive_ptr_release(T* iptr) noexcept {
	// retrieve resulting count right from the --count operator, so that std::atomic is properly used if iptr->refCount is atomic. 
	auto resultingCount = --iptr->refCount;

	if (resultingCount == 0)
		delete iptr;
}

/*
	Defines a managed pointer to a shared object, of which unlike shared_ptr, the refCount is stored in the object itself. 
	Requires the managed object to define a public (optionally atomic) integer-like field called 'refCount'. 
*/
template<typename T>
class intrusive_ptr {
protected:
	template<typename U> friend class intrusive_ptr;

	T* ptr;

public:
	intrusive_ptr() noexcept : ptr(nullptr) {}

	explicit intrusive_ptr(T* ptr, bool addRef = true) noexcept : ptr(ptr) {
		if (ptr && addRef)
			intrusive_ptr_add_ref(ptr);
	}

	intrusive_ptr(const intrusive_ptr& iptr) noexcept : ptr(iptr.ptr) {
		if (ptr)
			intrusive_ptr_add_ref(ptr);
	}

	intrusive_ptr(intrusive_ptr&& iptr) noexcept : ptr(iptr.ptr) {
		iptr.ptr = nullptr;
	}

	~intrusive_ptr() noexcept {
		if (ptr)
			intrusive_ptr_release(ptr);
	}

	intrusive_ptr& operator=(const intrusive_ptr& iptr) noexcept {
		return operator=(iptr.ptr);
	}

	intrusive_ptr& operator=(intrusive_ptr&& iptr) noexcept {
		swap(iptr);
		return *this;
	}

	template<typename U>
	intrusive_ptr& operator=(const intrusive_ptr<U>& iptr) noexcept {
		return operator=(iptr.ptr);
	}

	intrusive_ptr& operator=(T* ptr) noexcept {
		if (ptr != this->ptr) {
			const T* tmp = this->ptr;
			if (ptr)
				intrusive_ptr_add_ref(ptr);
			this->ptr = ptr;
			if (tmp)
				intrusive_ptr_release(tmp);
		}

		return *this;
	}

	[[nodiscard]] T& operator*() const noexcept {
		return *ptr;
	}

	[[nodiscard]] T* get() const noexcept {
		return ptr;
	}

	[[nodiscard]] T* operator->() const noexcept {
		return ptr;
	}

	[[nodiscard]] bool valid() const noexcept {
		return ptr != nullptr;
	}

	[[nodiscard]] bool invalid() const noexcept {
		return ptr == nullptr;
	}

	void reset() noexcept {
		T* const tmp = ptr;
		ptr = nullptr;
		if (tmp)
			intrusive_ptr_release(tmp);
	}

	void swap(intrusive_ptr& iptr) noexcept {
		T* const tmp = ptr;
		ptr = iptr.ptr;
		iptr.ptr = tmp;
	}

	void attach(T* ptr) noexcept {
		T* const tmp = this->ptr;
		this->ptr = ptr;
		if (tmp)
			intrusive_ptr_release(tmp);
	}

	[[nodiscard]] T* detach() noexcept {
		T* const tmp = ptr;
		ptr = nullptr;
		return tmp;
	}

	[[nodiscard]] operator bool() const noexcept {
		return ptr != nullptr;
	}

	[[nodiscard]] bool operator!() const noexcept {
		return ptr == nullptr;
	}
};

template <typename T, typename U>
bool operator==(const intrusive_ptr<T>& iptr1, const intrusive_ptr<U>& iptr2) noexcept {
	return (iptr1.get() == iptr2.get());
}

template <typename T, typename U>
bool operator!=(const intrusive_ptr<T>& iptr1, const intrusive_ptr<U>& iptr2) noexcept {
	return (iptr1.get() != iptr2.get());
}

template <typename T>
bool operator==(const intrusive_ptr<T>& iptr, T* ptr) noexcept {
	return (iptr.get() == ptr);
}

template <typename T>
bool operator!=(const intrusive_ptr<T>& iptr, T* ptr) noexcept {
	return (iptr.get() != ptr);
}

template <typename T>
bool operator==(T* ptr, const intrusive_ptr<T>& iptr) noexcept {
	return (ptr == iptr.get());
}

template <typename T>
bool operator!=(T* ptr, const intrusive_ptr<T>& iptr) noexcept {
	return (ptr != iptr.get());
}

template <typename T>
bool operator==(std::nullptr_t nptr, const intrusive_ptr<T>& iptr) noexcept {
	return (nullptr == iptr.get());
}

template <typename T>
bool operator!=(std::nullptr_t nptr, const intrusive_ptr<T>& iptr) noexcept {
	return (nullptr != iptr.get());
}

template <typename T>
bool operator==(const intrusive_ptr<T>& iptr, std::nullptr_t nptr) noexcept {
	return (iptr.get() == nullptr);
}

template <typename T>
bool operator!=(const intrusive_ptr<T>& iptr, std::nullptr_t nptr) noexcept {
	return (iptr.get() != nullptr);
}

template<typename T, typename... Args>
intrusive_ptr<T> make_intrusive(Args&&... args) noexcept {
	return intrusive_ptr<T>(new T(std::forward<Args>(args)...));
}

template <class T, class U>
intrusive_ptr<T> intrusive_cast(const intrusive_ptr<U>& iptr) noexcept {
	return intrusive_ptr<T>(static_cast<T*>(iptr.get()));
}

template<typename T>
using IRef = intrusive_ptr<T>;

template<typename T>
using SRef = std::shared_ptr<T>;

template<typename T>
using URef = std::unique_ptr<T>;

}