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
	T* begin() { return start; }
	T* end() { return fin; }
};
template<typename T>
struct ConstListIter {
	const T* start;
	const T* fin;
	const T* begin() const { return start; }
	const T* end() const { return fin; }
};

template<typename T>
struct BufferWithCapacity;

template<typename T>
void swapBuffers(BufferWithCapacity<T>& first, BufferWithCapacity<T>& second);

template<typename T>
struct BufferWithCapacity {
	T* data;
	size_t capacity;

	BufferWithCapacity() : BufferWithCapacity(16) {};
	BufferWithCapacity(size_t initialCapacity) : data(new T[initialCapacity]), capacity(initialCapacity) {}
	~BufferWithCapacity() {
		delete[] data;
	}
	BufferWithCapacity(const BufferWithCapacity& other) = delete;
	void operator=(const BufferWithCapacity& other) = delete;
	BufferWithCapacity(const BufferWithCapacity&& other) {
		swapBuffers(*this, other);
	}
	void operator=(const BufferWithCapacity&& other) {
		swapBuffers(*this, other);
	}
	void ensureCapacity(size_t newCapacity) {
		if (newCapacity > capacity) {
			newCapacity = nextPowerOf2(newCapacity);
			//Log::debug("Extending %s buffer capacity to %d", typeid(T).name(), newCapacity);
			T* newBuf = new T[newCapacity];
			for (int i = 0; i < capacity; i++) {
				newBuf[i] = data[i];
			}
			delete[] data;
			data = newBuf;
			capacity = newCapacity;
		}
	}
};

template<typename T>
void swapBuffers(BufferWithCapacity<T>& first, BufferWithCapacity<T>& second) {
	T* tmpBuf = first.data;
	size_t tmpSize = first.capacity;
	first.data = second.data;
	first.capacity = second.capacity;
	second.data = tmpBuf;
	second.capacity = tmpSize;
}

template<typename T>
struct AddableBuffer : public BufferWithCapacity<T> {
	size_t size = 0;

	AddableBuffer() : BufferWithCapacity<T>(16) {}
	AddableBuffer(size_t initialCapacity) : BufferWithCapacity<T>(initialCapacity) {}

	AddableBuffer(T* data, size_t dataSize, size_t initialCapacity) : BufferWithCapacity<T>(initialCapacity), size(dataSize) {
		if (data == nullptr) Log::fatal("Could not create AddableBuffer of size: %d", initialCapacity);
		memcpy(this->data, data, dataSize * sizeof(T));
	}

	~AddableBuffer() {
	}

	AddableBuffer(const AddableBuffer&) = delete;
	AddableBuffer& operator=(const AddableBuffer&) = delete;

	AddableBuffer(AddableBuffer && other) {
		swapBuffers(*this, other);
	}
	AddableBuffer& operator=(AddableBuffer && other) {
		swapBuffers(*this, other);
	}

	inline void add(const T & obj) {
		BufferWithCapacity<T>::ensureCapacity(size + 1);
		BufferWithCapacity<T>::data[size++] = obj;
	}

	inline void clear() {
		this->size = 0;
	}

	inline T* begin() { return data; }
	inline const T* begin() const { return data; }

	inline T* end() { return data + size; }
	inline const T* end() const { return data + size; }
};
