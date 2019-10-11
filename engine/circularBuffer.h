#pragma once


template<typename T, size_t N>
struct CircularBuffer {
	T buf[N];
	size_t curI = 0;
	bool hasComeAround = false;

	inline void add(const T& newObj) {
		buf[curI] = newObj;
		curI++;

		if(curI >= N) {
			curI = 0;
			hasComeAround = true;
		}
	}

	inline T sum() const {
		size_t limit = size();

		if(limit == 0)
			return T();

		T total = buf[0];

		for(size_t i = 1; i < limit; i++)
			total += buf[i];
		
		return total;
	}

	inline T avg() const {
		size_t limit = size();

		if(limit == 0)
			return T();

		T total = sum();

		return T(total / limit);
	}

	inline size_t size() const {
		if(hasComeAround)
			return N;
		else
			return curI;
	}

	inline T* begin() {
		return &buf[0];
	}

	inline T* end() {
		return &buf[0] + size();
	}
};
