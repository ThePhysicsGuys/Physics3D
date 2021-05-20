#pragma once

#include "../physics/datastructures/buffers.h"

#include <mutex>

namespace P3D {
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

	inline void add(const T& obj) {
		writeBuf.add(obj);
	}

	inline void pushWriteBuffer() {
		std::lock_guard<std::mutex> lg(swapLock);

		readySize = writeBuf.size;
		std::swap(static_cast<BufferWithCapacity<T>&>(writeBuf), readyBuf);
		writeBuf.clear();

		newDataAvailable = true;
	}

	inline AddableBuffer<T>& pullOutputBuffer() {
		std::lock_guard<std::mutex> lg(swapLock);

		if(newDataAvailable) {
			std::swap(static_cast<BufferWithCapacity<T>&>(readyBuf), static_cast<BufferWithCapacity<T>&>(outputBuf));

			newDataAvailable = false;
		}

		outputBuf.size = readySize;

		return outputBuf;
	}
};
};
