#pragma once

#include "buffers.h"

template<typename T>
class SplitUnorderedList : public AddableBuffer<T> {
	
private:
	inline void pushToEnd(const T& newItem, T&& oldItem) {
		add(std::move(oldItem));
		oldItem = newItem;
	}
	inline void pushToEnd(T&& newItem, T&& oldItem) {
		add(std::move(oldItem));
		oldItem = std::move(newItem);
	}
public:

	size_t splitOffset;
	
	SplitUnorderedList() : SplitUnorderedList<T>(16) {}
	SplitUnorderedList(size_t initialCapacity) : AddableBuffer<T>(initialCapacity), splitOffset(0) {}

	inline T* addLeftSide(const T& newObj) {
		if (splitOffset != size) {
			pushToEnd(newObj, std::move(data[splitOffset]));
		} else {
			add(newObj);
		}
		return data + splitOffset++;
	}

	inline T* addLeftSide(T&& newObj) {
		if (splitOffset != size) {
			pushToEnd(std::move(newObj), std::move(data[splitOffset]));
		} else {
			add(std::move(newObj));
		}
		return data + splitOffset++;
	}

	inline void remove(T* item) {
		if (item < data + splitOffset) {
			*item = std::move(data[splitOffset]);
			data[splitOffset] = std::move(data[--size]);
			splitOffset--;
		} else {
			*item = std::move(data[--size]);
		}
	}

	inline void moveLeftToRight(T* obj) {
		splitOffset--;
		std::swap(*obj, data[splitOffset]);
	}

	inline void moveRightToLeft(T* obj) {
		std::swap(*obj, data[splitOffset]);
		splitOffset++;
	}

	inline bool isLeftSide(const T* obj) const {
		return obj < data + splitOffset;
	}

	inline bool isRightSide(const T* obj) const {
		return obj >= data + splitOffset;
	}

	inline ListIter<T> iterLeft() { return ListIter<T>{ data, data + splitOffset }; }
	inline ConstListIter<T> iterLeft() const { return ConstListIter<T>{ data, data + splitOffset }; }
	inline ListIter<T> iterRight() { return ListIter<T>{ data + splitOffset, data+size }; }
	inline ConstListIter<T> iterRight() const { return ConstListIter<T>{ data + splitOffset, data + size }; }
};
