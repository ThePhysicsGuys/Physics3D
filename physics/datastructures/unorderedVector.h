#pragma once

#include <vector>
#include <assert.h>

template<typename T>
class UnorderedVector : public std::vector<T> {
public:
	using std::vector<T>::vector;

	inline void remove(T&& element) {
		assert(&element >= &std::vector<T>::front());
		
		T& bck = std::vector<T>::back();
		
		assert(&element <= &bck);

		if(&element != &bck) {
			element = std::move(bck);
		}
		std::vector<T>::pop_back();
	}
};
