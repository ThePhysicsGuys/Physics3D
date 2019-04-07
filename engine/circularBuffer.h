#pragma once


template<typename T, size_t N>
struct CircularBuffer {
	T buf[N];
	size_t curI = 0;
	bool hasComeAround = false;

	inline void add(T& newObj) {
		buf[curI] = newObj;
		curI++;
		if(curI >= N) {
			curI = 0;
			hasComeAround = true;
		}
	}
	inline T sum() const {
		size_t limit;
		if(hasComeAround) {
			limit = N;
		} else {
			if(curI == 0) return T();
			limit = curI;
		}
		T total = buf[0];
		for(size_t i = 1; i < limit; i++) {
			total += buf[i];
		}
		return total;
	}
	inline T avg() const {
		size_t limit;
		if(hasComeAround) {
			limit = N;
		} else {
			if(curI == 0) return T();
			limit = curI;
		}
		T total = sum();
		return total / limit;
	}
};
