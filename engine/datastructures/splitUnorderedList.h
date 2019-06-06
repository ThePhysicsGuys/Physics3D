#pragma once

#include "buffers.h"

template<typename T>
class SplitUnorderedList : public AddableBuffer<T> {
	
private:
	inline void pushToEnd(const T& newItem, T&& oldItem) {
		this->add(std::move(oldItem));
		oldItem = newItem;
	}
	inline void pushToEnd(T&& newItem, T&& oldItem) {
		this->add(std::move(oldItem));
		oldItem = std::move(newItem);
	}
public:

	size_t splitOffset;
	
	SplitUnorderedList() : SplitUnorderedList<T>(16) {}
	SplitUnorderedList(size_t initialCapacity) : AddableBuffer<T>(initialCapacity), splitOffset(0) {}

	inline T* addLeftSide(const T& newObj) {
		if (splitOffset != this->size) {
			pushToEnd(newObj, std::move(this->data[splitOffset]));
		} else {
			this->add(newObj);
		}
		return this->data + splitOffset++;
	}

	inline T* addLeftSide(T&& newObj) {
		if (splitOffset != this->size) {
			pushToEnd(std::move(newObj), std::move(this->data[splitOffset]));
		} else {
			this->add(std::move(newObj));
		}
		return this->data + splitOffset++;
	}

	inline void remove(T* item) {
		if (item < this->data + splitOffset) {
			*item = std::move(this->data[splitOffset]);
			this->data[splitOffset] = std::move(this->data[--this->size]);
			splitOffset--;
		} else {
			*item = std::move(this->data[--this->size]);
		}
	}

	inline void moveLeftToRight(T* obj) {
		splitOffset--;
		std::swap(*obj, this->data[splitOffset]);
	}

	inline void moveRightToLeft(T* obj) {
		std::swap(*obj, this->data[splitOffset]);
		splitOffset++;
	}

	inline bool isLeftSide(const T* obj) const {
		return obj < this->data + splitOffset;
	}

	inline bool isRightSide(const T* obj) const {
		return obj >= this->data + splitOffset;
	}

	inline ListIter<T> iterLeft() { return ListIter<T>{ this->data, this->data + splitOffset }; }
	inline ConstListIter<T> iterLeft() const { return ConstListIter<T>{ this->data, this->data + splitOffset }; }
	inline ListIter<T> iterRight() { return ListIter<T>{ this->data + splitOffset, this->data + this->size }; }
	inline ConstListIter<T> iterRight() const { return ConstListIter<T>{ this->data + splitOffset, this->data + this->size }; }
};
