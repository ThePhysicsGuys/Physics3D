#pragma once

#include <vector>
#include <assert.h>

namespace P3D {
template<typename T>
class UnorderedVector : public std::vector<T> {
public:

	inline void remove(T&& element) {
		T* el = &element;
		T* frnt = &std::vector<T>::front();
		assert(el >= frnt);

		T* bck = &std::vector<T>::back();

		assert(el <= bck);

		if(el != bck) {
			*el = std::move(*bck);
		}
		std::vector<T>::pop_back();
	}
};
};
