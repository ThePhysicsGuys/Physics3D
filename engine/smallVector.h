#pragma once

#include <algorithm>

template<typename T, int LocalBufSize>
class SmallVector {
public:
	int size = 0;

	constexpr int sharedSize() { return sizeof(T*) / sizeof(T); }
	constexpr int guaranteedLocalSize() {return LocalBufSize - sharedSize();}

	T localData[LocalBufSize - sizeof(T*) / sizeof(T)];
	union {
		T* remoteData;	// ptr last, more likely to align to cache bound
		T sharedData[sizeof(T*) / sizeof(T)];
	};

public:
	bool hasExternalBuf() const { return size > LocalBufSize; }

	SmallVector() = default;

	SmallVector(const T* initialData, int N) : size(N) {
		if(hasExternalBuf()){
			remoteData = new T[N - guaranteedLocalSize()];
		}
		for(int i = 0; i < N; i++) {
			(*this)[i] = initialData[i];
		}
	}
	~SmallVector() {
		if(hasExternalBuf()) {
			delete[] remoteData;
		}
	}
	void setData(const T* newData, int N) {
		if(hasExternalBuf()) {
			delete[] remoteData;
		}
		size = N;
		if(hasExternalBuf()) {
			remoteData = new T[N - guaranteedLocalSize()];
		}
		for(int i = 0; i < N; i++) {
			(*this)[i] = newData[i];
		}
	}
	const T& operator[](int index) const {
		if(index < guaranteedLocalSize()) {
			return localData[index];
		} else {
			if(this->size <= LocalBufSize) {
				return sharedData[index - guaranteedLocalSize()];
			} else {
				return remoteData[index - guaranteedLocalSize()];
			}
		}
	}
	T& operator[](int index) {
		if(index < guaranteedLocalSize()) {
			return localData[index];
		} else {
			if(this->size <= LocalBufSize) {
				return sharedData[index - guaranteedLocalSize()];
			} else {
				return remoteData[index - guaranteedLocalSize()];
			}
		}
	}
};
