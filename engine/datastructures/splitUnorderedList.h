#pragma once

#include "buffers.h"

template<typename T>
class SplitUnorderedList : public AddableBuffer<T> {
	
private:
	inline void pushToEnd(const T& newItem, T& oldItem) {
		add(oldItem);
		oldItem = newItem;
	}
public:
	T* splitOffset;
	
	SplitUnorderedList() : SplitUnorderedList<T>(16) {}
	SplitUnorderedList(size_t initialCapacity) : AddableBuffer<T>(initialCapacity), splitOffset(data) {}

	inline T* addLeftSide(const T& newObj) {
		if (splitOffset != data + size) {
			pushToEnd(newObj, *splitOffset);
		} else {
			add(newObj);
		}
		return splitOffset++;
	}

	inline void remove(T* item) {
		if (item < splitOffset) {
			*item = *splitOffset;
			*splitOffset = data[--size];
			splitOffset--;
		} else {
			*item = data[--size];
		}
	}

	inline void moveLeftToRight(T* obj) {
		splitOffset--;
		T tmp = *obj;
		*obj = *splitOffset;
		*splitOffset = tmp;
	}

	inline void moveRightToLeft(T* obj) {
		T tmp = *obj;
		*obj = *splitOffset;
		*splitOffset = tmp;
		splitOffset++;
	}

	inline bool isLeftSide(const T* obj) const {
		return obj < splitOffset;
	}

	inline bool isRightSide(const T* obj) const {
		return obj >= splitOffset;
	}

	inline ListIter<T> iterLeft() { return ListIter<T>{ data, splitOffset }; }
	inline ConstListIter<T> iterLeft() const { return ConstListIter<T>{ data, splitOffset }; }
	inline ListIter<T> iterRight() { return ListIter<T>{ splitOffset, data+size }; }
	inline ConstListIter<T> iterRight() const { return ConstListIter<T>{ splitOffset, data + size }; }
};
