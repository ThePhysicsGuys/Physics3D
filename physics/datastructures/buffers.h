#pragma once

#include "iteratorEnd.h"

inline unsigned long long nextPowerOf2(unsigned long long v) {
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

	BufferWithCapacity(BufferWithCapacity&& other) noexcept {
		this->data = other.data;
		this->capacity = other.capacity;
		other.data = nullptr;
		other.capacity = 0;
	}

	~BufferWithCapacity() {
		delete[] data;
	}

	BufferWithCapacity(const BufferWithCapacity& other) = delete;
	void operator=(const BufferWithCapacity& other) = delete;

	BufferWithCapacity& operator=(BufferWithCapacity&& other) noexcept {
		std::swap(this->data, other.data);
		std::swap(this->capacity, other.capacity);
		return *this;
	}

	void resize(size_t newCapacity, size_t sizeToCopy) {
		T* newBuf = new T[newCapacity];
		for (size_t i = 0; i < sizeToCopy; i++) 
			newBuf[i] = std::move(data[i]);
		
		delete[] data;
		data = newBuf;
		capacity = newCapacity;
	}

	size_t ensureCapacity(size_t newCapacity, size_t sizeToCopy) {
		if (newCapacity > this->capacity) {
			size_t nextPower = nextPowerOf2(newCapacity);
			resize(nextPower, sizeToCopy);
			return nextPower;
		}

		return this->capacity;
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
		if (data == nullptr)
			Log::fatal("Could not create AddableBuffer of size: %d", initialCapacity);
		
		for (size_t i = 0; i < dataSize; i++)
			this->data[i] = data[i];
	}

	~AddableBuffer() {}

	AddableBuffer(const AddableBuffer<T>&) = delete;
	AddableBuffer& operator=(const AddableBuffer<T>&) = delete;

	AddableBuffer(AddableBuffer<T>&& other) noexcept : BufferWithCapacity<T>(std::move(other)), size(other.size) {
		other.size = 0;
	}

	AddableBuffer<T>& operator=(AddableBuffer<T>&& other) noexcept {
		BufferWithCapacity<T>::operator=(std::move(other));
		size_t tmpSize = this->size;
		this->size = other.size;
		other.size = tmpSize;
		return *this;
	}

	inline void resize(size_t newCapacity) {
		BufferWithCapacity<T>::resize(newCapacity, this->size);
	}

	inline size_t ensureCapacity(size_t newCapacity) {
		size_t newSize = BufferWithCapacity<T>::ensureCapacity(newCapacity, this->size);
		return newSize;
	}

	inline T* add(const T& obj) {
		this->ensureCapacity(this->size + 1);
		this->data[this->size] = obj;
		return &this->data[this->size++];
	}

	inline T* add(T&& obj) {
		this->ensureCapacity(this->size + 1);
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

template<typename T>
struct CircularIter {
	T* iter;
	T* bufStart;
	T* bufEnd;
	size_t itemsLeft;

	void operator++() {
		iter++;
		if(iter == bufEnd) {
			iter = bufStart;
		}
		itemsLeft--;
	}

	T& operator*() const { return *iter; }

	bool operator==(IteratorEnd) const {
		return this->itemsLeft == 0;
	}

	bool operator!=(IteratorEnd) const {
		return this->itemsLeft != 0;
	}
};

template<typename T>
struct CircularBuffer {
private:
	T* buf;
	size_t curI = 0;
	size_t capacity;
	bool hasComeAround = false;
public:
	CircularBuffer() : buf(nullptr), capacity(0) {}
	CircularBuffer(size_t capacity) : buf(new T[capacity+1]), capacity(capacity) {}
	~CircularBuffer() { delete[] buf; }
	
	inline size_t size() const {
		if(hasComeAround)
			return capacity;
		else
			return curI;
	}

	CircularBuffer(const CircularBuffer<T>& other) : buf(new T[other.capacity+1]), curI(other.curI), capacity(other.capacity), hasComeAround(other.hasComeAround) {
		for(size_t i = 0; i < other.capacity; i++) {
			this->buf[i] = other.buf[i];
		}
	}
	CircularBuffer& operator=(const CircularBuffer<T>& other) {
		if(this->capacity != other.capacity) {
			delete[] this->buf;
			this->buf = new T[other.capacity];
		}

		this->curI = other.curI;
		this->capacity = other.capacity;
		this->hasComeAround = other.hasComeAround;
		for(size_t i = 0; i < other.capacity; i++) {
			this->buf[i] = other.buf[i];
		}
		return *this;
	}
	CircularBuffer(CircularBuffer<T>&& other) : buf(other.buf), curI(other.curI), capacity(other.capacity), hasComeAround(other.hasComeAround) {
		other.buf = nullptr;
		other.capacity = 0;
		other.curI = 0; 
		other.hasComeAround = false;
	}
	CircularBuffer& operator=(CircularBuffer<T>&& other) {
		std::swap(this->buf, other.buf);
		std::swap(this->curI, other.curI);
		std::swap(this->capacity, other.capacity);
		std::swap(this->hasComeAround, other.hasComeAround);
		return *this;
	}

	inline void add(const T& newObj) {
		buf[curI] = newObj;
		curI++;

		if (curI >= capacity) {
			curI = 0;
			hasComeAround = true;
		}
	}

	inline T sum() const {
		size_t limit = size();

		if (limit == 0)
			return T();

		T total = buf[0];

		for (size_t i = 1; i < limit; i++)
			total += buf[i];

		return total;
	}

	inline T avg() const {
		size_t limit = size();

		if (limit == 0)
			return T();

		T total = sum();

		return T(total / limit);
	}

	inline void resize(size_t newCapacity) {
		T* newBuf = new T[newCapacity];

		T* cur = newBuf;

		size_t elementsForgotten = (this->capacity > newCapacity) ? this->capacity - newCapacity : 0;

		if(hasComeAround) {
			for(size_t i = curI + elementsForgotten; i < capacity; i++) {
				*cur = buf[i];
				cur++;
			}

			if(elementsForgotten > capacity - curI) {
				elementsForgotten -= (capacity - curI);
			}
		}
		for(size_t i = elementsForgotten; i < curI; i++) {
			*cur = buf[i];
			cur++;
		}

		delete[] buf;
		this->buf = newBuf;
	}

	T& front() {
		return (curI == 0) ? buf[capacity-1] : buf[curI-1];
	}

	const T& front() const {
		return (curI == 0) ? buf[capacity - 1] : buf[curI - 1];
	}

	T& tail() {
		return hasComeAround ? buf[curI] : buf[0];
	}

	const T& tail() const {
		return hasComeAround ? buf[curI] : buf[0];
	}

	inline CircularIter<T> begin() {
		return CircularIter<T>{hasComeAround? buf + curI : buf, buf, buf+capacity, size()};
	}

	inline CircularIter<const T> begin() const {
		return CircularIter<const T>{hasComeAround ? buf + curI : buf, buf, buf + capacity, size()};
	}

	inline IteratorEnd end() {
		return IteratorEnd();
	}
};
