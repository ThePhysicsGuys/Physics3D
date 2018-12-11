#pragma once

#include <mutex>

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
	SwappableBuffer& operator=(const SwappableBuffer&) = delete;


	void add(const T& obj) {
		if (writeIndex == writeCapacity) {
			writeCapacity *= 2;
			T* newPtr = (T*)realloc(writeBuf, sizeof(T) * writeCapacity);
			if (newPtr == nullptr) {
				Log::fatal("Could not extend write buffer to size: %d", writeCapacity);
			} else {
				Log::info("Extended write buffer to: %d", writeCapacity);
				writeBuf = newPtr;
			}
		}

		writeBuf[writeIndex++] = obj;
	}

	void swap() {
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

	void lockRead() { readLock.lock(); }
	void unlockRead() { readLock.unlock(); }
};
