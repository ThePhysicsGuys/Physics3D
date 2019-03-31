#pragma once

#include <mutex>

template<typename T>
class AddableBuffer;

template<typename T>
class SwappableBuffer {
	T* writeBuf;
	size_t writeIndex = 0;
	size_t writeCapacity;
	size_t readCapacity;
	std::mutex readLock;
public:
	T* readData;
	size_t readSize;

	SwappableBuffer(int initialCapacity) : writeCapacity(initialCapacity), writeBuf((T*)malloc(initialCapacity * sizeof(T))),
		readCapacity(initialCapacity), readData((T*)malloc(initialCapacity * sizeof(T))) {
		if (readData == nullptr || writeBuf == nullptr)
			Log::fatal("Could not create SwappableBuffer of size: %d", initialCapacity);
	}

	~SwappableBuffer() {
		lockRead();
		free(writeBuf);
		free(readData);
		unlockRead();
	}

	SwappableBuffer(const SwappableBuffer&) = delete;
	SwappableBuffer(const SwappableBuffer&&) = delete;
	SwappableBuffer& operator=(const SwappableBuffer&) = delete;
	SwappableBuffer& operator=(const SwappableBuffer&&) = delete;

	inline void add(const T& obj) {
		if (this->writeIndex >= this->writeCapacity) {
			this->writeCapacity *= 2;
			T* newPtr = (T*)realloc(writeBuf, sizeof(T) * this->writeCapacity);
			if (newPtr == nullptr) {
				Log::fatal("Could not extend write buffer to size: %d", this->writeCapacity);
			} else {
				Log::info("Extended write buffer to: %d", this->writeCapacity);
				writeBuf = newPtr;
			}
		}

		writeBuf[writeIndex++] = obj;
	}

	inline void swap() {
		lockRead();

		T* tmpPtr = readData;
		size_t tmpSize = readCapacity;

		readData = writeBuf;
		readCapacity = writeCapacity;
		readSize = writeIndex;

		writeBuf = tmpPtr;
		writeCapacity = tmpSize;
		writeIndex = 0;

		unlockRead();
	}

	inline AddableBuffer<T> getReadBuffer() {
		lockRead();
		AddableBuffer<T> b(readData, readSize, readCapacity);
		unlockRead();
		return b;
	}

	inline void lockRead() { readLock.lock(); }
	inline void unlockRead() { readLock.unlock(); }
};

template<typename T>
class AddableBuffer {
public:
	T* data;
	size_t index;
private:
	size_t capacity;
public:

	AddableBuffer(int initialCapacity) : capacity(initialCapacity), data((T*)malloc(initialCapacity * sizeof(T))), index(0) {
		if (data == nullptr) Log::fatal("Could not create AddableBuffer of size: %d", initialCapacity);
	}

	AddableBuffer(T* data, int dataSize, int initialCapacity) : capacity(initialCapacity), data((T*)malloc(initialCapacity * sizeof(T))), index(dataSize) {
		if (data == nullptr) Log::fatal("Could not create AddableBuffer of size: %d", initialCapacity);
		memcpy(this->data, data, dataSize * sizeof(T));
	}

	~AddableBuffer() {
		free(data);
	}

	AddableBuffer(const AddableBuffer&) = delete;
	AddableBuffer& operator=(const AddableBuffer&) = delete;

	AddableBuffer(AddableBuffer&& buf) {
		size_t oldCapacity = capacity;
		size_t oldIndex = index;
		T* oldData = data;
		this->capacity = buf.capacity;
		this->index = buf.index;
		this->data = buf.data;
		buf.capacity = oldCapacity;
		buf.index = oldIndex;
		buf.data = oldData;
	}
	AddableBuffer& operator=(AddableBuffer&& buf) {
		size_t oldCapacity = capacity;
		size_t oldIndex = index;
		T* oldData = data;
		this->capacity = buf.capacity;
		this->index = buf.index;
		this->data = buf.data;
		buf.capacity = oldCapacity;
		buf.index = oldIndex;
		buf.data = oldData;
	}
	
	inline void add(const T& obj) {
		if (index == capacity) {
			capacity *= 2;
			T* newPtr = (T*)realloc(data, sizeof(T) * capacity);
			if (newPtr == nullptr) {
				Log::fatal("Could not extend write buffer to size: %d", capacity);
			} else {
				Log::info("Extended write buffer to: %d", capacity);
				data = newPtr;
			}
		}

		data[index++] = obj;
	}

	inline void clear() {
		this->index = 0;
	}
};
