#pragma once

inline unsigned long nextPowerOf2(unsigned long v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v |= v >> 32;
	return v + 1;
}

template<typename T>
struct ListIter {
	T* start;
	T* fin;
	T* begin() const { return start; }
	T* end() const { return fin; }
};
/*template<typename T>
struct ConstListIter {
	const T* start;
	const T* fin;
	const T* begin() const { return start; }
	const T* end() const { return fin; }
};*/

template<typename T>
struct ListOfPtrIter {
	T* const* cur;
	ListOfPtrIter() = default;
	ListOfPtrIter(T* const* cur) : cur(cur) {}
	T& operator*() const {
		return **cur;
	}
	ListOfPtrIter& operator++() {
		cur++;
		return *this;
	}
	ListOfPtrIter operator++(int) {
		ListOfPtrIter prevSelf = *this;
		cur++;
		return prevSelf;
	}
	bool operator==(const ListOfPtrIter& other) const {
		return this->cur == other.cur;
	}
	bool operator!=(const ListOfPtrIter& other) const {
		return this->cur != other.cur;
	}
	T* operator->() const {
		return *cur;
	}
	operator T* () const {
		return *cur;
	}
};

template<typename T>
struct ListOfPtrIterFactory {
	T* const * start;
	T* const * fin;
	ListOfPtrIterFactory() {}
	ListOfPtrIterFactory(T* const * start, T* const * fin) : start(start), fin(fin) {}
	ListOfPtrIterFactory(const ListIter<T * const>& iter) : start(iter.start), fin(iter.fin) {}
	ListOfPtrIterFactory(const ListIter<T *>& iter) : start(iter.start), fin(iter.fin) {}

	ListOfPtrIter<T> begin() const { return ListOfPtrIter<T>{start}; }
	ListOfPtrIter<T> end() const { return ListOfPtrIter<T>{fin}; }
};

template<typename T>
struct BufferWithCapacity {
	T* data;
	size_t capacity;

	BufferWithCapacity() : data(nullptr), capacity(0) {};
	BufferWithCapacity(size_t initialCapacity) : data(new T[initialCapacity]), capacity(initialCapacity) {}
	~BufferWithCapacity() {
		delete[] data;
	}
	BufferWithCapacity(const BufferWithCapacity& other) = delete;
	void operator=(const BufferWithCapacity& other) = delete;
	BufferWithCapacity(BufferWithCapacity&& other) noexcept {
		this->data = other.data;
		this->capacity = other.capacity;
		other.data = nullptr;
		other.capacity = 0;
	}
	BufferWithCapacity& operator=(BufferWithCapacity&& other) noexcept {
		std::swap(this->data, other.data);
		std::swap(this->capacity, other.capacity);
		return *this;
	}
	void resize(size_t newCapacity, size_t sizeToCopy) {
		//Log::debug("Extending %s buffer capacity to %d", typeid(T).name(), newCapacity);
		T* newBuf = new T[newCapacity];
		for (int i = 0; i < sizeToCopy; i++) {
			newBuf[i] = std::move(data[i]);
		}
		delete[] data;
		data = newBuf;
		capacity = newCapacity;
	}
	void ensureCapacity(size_t newCapacity, size_t sizeToCopy) {
		if (newCapacity > capacity) {
			resize(nextPowerOf2(newCapacity), sizeToCopy);
		}
	}
	T& operator[](size_t index) { return data[index]; }
	const T& operator[](size_t index) const { return data[index]; }
};

template<typename T>
struct AddableBuffer : public BufferWithCapacity<T> {
	size_t size = 0;

	AddableBuffer() : BufferWithCapacity<T>() {}
	AddableBuffer(size_t initialCapacity) : BufferWithCapacity<T>(initialCapacity) {}

	AddableBuffer(T* data, size_t dataSize, size_t initialCapacity) : BufferWithCapacity<T>(initialCapacity), size(dataSize) {
		if (data == nullptr) Log::fatal("Could not create AddableBuffer of size: %d", initialCapacity);
		for (size_t i = 0; i < dataSize; i++) {
			this->data[i] = data[i];
		}
	}

	~AddableBuffer() {
	}

	AddableBuffer(const AddableBuffer<T>&) = delete;
	AddableBuffer& operator=(const AddableBuffer<T>&) = delete;

	AddableBuffer(AddableBuffer<T> && other) noexcept : BufferWithCapacity<T>(std::move(other)), size(other.size) {
		other.size = 0;
	}
	AddableBuffer<T>& operator=(AddableBuffer<T> && other) noexcept {
		BufferWithCapacity<T>::operator=(std::move(other));
		size_t tmpSize = this->size;
		this->size = other.size;
		other.size = tmpSize;
		return *this;
	}

	inline T* add(const T& obj) {
		this->ensureCapacity(this->size + 1, this->size);
		this->data[this->size] = obj;
		return &this->data[this->size++];
	}

	inline T* add(T&& obj) {
		this->ensureCapacity(this->size + 1, this->size);
		this->data[this->size] = std::move(obj);
		return &this->data[this->size++];
	}

	inline void clear() {
		size = 0;
	}

	inline T* begin() { return this->data; }
	inline const T* begin() const { return this->data; }

	inline T* end() { return this->data + size; }
	inline const T* end() const { return this->data + this->size; }

	inline bool liesInList(const T* obj) const {
		return obj >= this->data && obj < this->data + this->size;
	}
};

template<typename T, size_t N>
struct FixedLocalBuffer {
	T buf[N];
	size_t size = 0;

	T& operator[](size_t index) { return buf[index]; }
	const T& operator[](size_t index) const { return buf[index]; }

	const T* begin() const { return &buf; }
	T* begin() { return &buf; }
	const T* end() const { return buf + size; }
	T* end() { return buf + size; }

	void add(const T& obj) {
		buf[size++] = obj;
	}
};
