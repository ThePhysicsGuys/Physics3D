#pragma once

#include <mutex>

template<typename T>
struct BufferWithCapacity;

template<typename T>
struct AddableBuffer;

template<typename T>
void swapBuffers(BufferWithCapacity<T>& first, BufferWithCapacity<T>& second);

inline unsigned long nextPowerOf2(unsigned long v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v |= v >> 32;
	return v+1;
}

template<typename T>
struct BufferWithCapacity {
	T* data;
	size_t capacity;

	BufferWithCapacity() : BufferWithCapacity(10) {};
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
	size_t index = 0;

	AddableBuffer() : BufferWithCapacity(10) {}
	AddableBuffer(size_t initialCapacity) : BufferWithCapacity(initialCapacity) {}

	AddableBuffer(T * data, size_t dataSize, size_t initialCapacity) : BufferWithCapacity(initialCapacity), index(dataSize) {
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
		ensureCapacity(index+1);

		data[index++] = obj;
	}

	inline void clear() {
		this->index = 0;
	}
};

template<typename T>
class ThreePhaseBuffer {
	AddableBuffer<T> writeBuf;
	BufferWithCapacity<T> readyBuf;
	size_t readySize = 0;
	bool newDataAvailable = false;
public:
	AddableBuffer<T> outputBuf;
	std::mutex swapLock;

	ThreePhaseBuffer(size_t initialCapacity) : writeBuf(initialCapacity), readyBuf(initialCapacity), outputBuf(initialCapacity) {}

	~ThreePhaseBuffer() {}

	ThreePhaseBuffer(const ThreePhaseBuffer&) = delete;
	ThreePhaseBuffer(const ThreePhaseBuffer&&) = delete;
	ThreePhaseBuffer& operator=(const ThreePhaseBuffer&) = delete;
	ThreePhaseBuffer& operator=(const ThreePhaseBuffer&&) = delete;

	inline void add(const T & obj) {
		writeBuf.add(obj);
	}

	inline void pushWriteBuffer() {
		std::lock_guard<std::mutex> lg(swapLock);

		readySize = writeBuf.index;
		swapBuffers(writeBuf, readyBuf);
		writeBuf.clear();

		newDataAvailable = true;
	}

	inline AddableBuffer<T>& pullOutputBuffer() {
		std::lock_guard<std::mutex> lg(swapLock);

		if (newDataAvailable) {
			swapBuffers(readyBuf, outputBuf);

			newDataAvailable = false;
		}

		outputBuf.index = readySize;

		return outputBuf;
	}
};
