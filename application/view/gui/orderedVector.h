#pragma once
#include "../util/log.h"

#include <algorithm>
#include <vector>

template<typename T>
struct OrderedVector {
private:
	std::vector<T> elements;
public:
	inline typename std::vector<T>::const_iterator begin() const {
		return elements.begin();
	}

	inline typename std::vector<T>::const_iterator end() const {
		return elements.end();
	}

	inline typename std::vector<T>::const_reverse_iterator rbegin() const {
		return elements.rbegin();
	}

	inline typename std::vector<T>::const_reverse_iterator rend() const {
		return elements.rend();
	}

	inline void insert(const T& element) {
		elements.insert(begin(), element);
	}

	inline void add(const T& element) {
		elements.push_back(element);
	}

	inline void remove(typename std::vector<T>::const_iterator iterator) {
		elements.erase(iterator);
	}

	inline void remove(const T& element) {
		for (auto iterator = begin(); iterator != end(); ++iterator) {
			if (&element == &*iterator)
				elements.erase(iterator);
		}
	}

	inline void select(const T& element) {
		for (auto iterator = begin(); iterator != end(); ++iterator) {
			if (element == *iterator) {
				auto resultValue = *iterator;
				elements.erase(iterator);
				elements.insert(begin(), resultValue);
				return;
			}
		}
	}

	inline size_t size() const {
		return elements.size();
	}

	T operator[](size_t index) const {
		return elements[index];
	}
};