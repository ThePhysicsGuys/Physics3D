#pragma once

#include "part.h"
#include "physical.h"
#include "datastructures/iterators.h"

class ConstPartIterator {
	ListOfPtrIter<const Physical> currentPhys;
	ConstPartIter currentPartInPhys;
public:
	inline ConstPartIterator(const ListOfPtrIter<const Physical>& currentPhys) : currentPhys(currentPhys), currentPartInPhys{ currentPhys->begin() } {}
	inline ConstPartIterator(const ListOfPtrIter<const Physical>& currentPhys, const ConstPartIter& currentPartInPhys) : currentPhys(currentPhys), currentPartInPhys(currentPartInPhys) {}
	inline const Part& operator*() const {
		return *currentPartInPhys;
	}

	inline ConstPartIterator& operator++() {
		++currentPartInPhys;
		if (currentPartInPhys == currentPhys->end()) {
			currentPhys++;
			currentPartInPhys = currentPhys->begin();
		}
		return *this;
	}

	inline bool operator!=(const ConstPartIterator& other) const { return this->currentPhys != other.currentPhys; }
};


typedef CompositeIterator<ListOfPtrIter<Physical>> PartIterator;


/*class PartIterator {
	ListOfPtrIter<Physical> currentPhys;
	PartIter currentPartInPhys;
public:
	inline PartIterator(const ListOfPtrIter<Physical>& startPhys) : currentPhys(startPhys), currentPartInPhys{ startPhys->begin() } {}
	inline PartIterator(const ListOfPtrIter<Physical>& startPhys, const PartIter& partInPhys) : currentPhys(startPhys), currentPartInPhys(partInPhys) {}

	inline Part& operator*() const {
		return *currentPartInPhys;
	}

	inline PartIterator& operator++() {
		++currentPartInPhys;
		return *this;
	}

	inline bool operator!=(const PartIterator& other) {
		if (currentPartInPhys == currentPhys->end()) {
			++currentPhys;
			if (currentPhys != other.currentPhys) {
				currentPartInPhys = currentPhys->begin();
				return true;
			} else {
				return false;
			}
		} else {
			return true;
		}
	}
};*/

class ConstPartIteratorFactory {
	ListOfPtrIter<const Physical> start;
	ListOfPtrIter<const Physical> finish;
public:
	inline ConstPartIteratorFactory(const ListOfPtrIter<const Physical>& start, const ListOfPtrIter<const Physical>& finish) : start(start), finish(finish) {}
	inline ConstPartIterator begin() const { return ConstPartIterator(start); }
	inline ConstPartIterator end() const { return ConstPartIterator(finish, ConstPartIter()); }
};

class PartIteratorFactory {
	ListOfPtrIter<Physical> start;
	ListOfPtrIter<Physical> finish;
public:
	inline PartIteratorFactory(ListOfPtrIter<Physical> start, ListOfPtrIter<Physical> finish) : start(start), finish(finish) {}
	inline PartIterator begin() const { return PartIterator(start); }
	inline PartIterator end() const { return PartIterator(finish, PartIter()); }
};


template<typename T> 
using CustomPartIter = CastingIterator<PartIterator, T&>;

/*template<typename T>
class CustomPartIter {
	PartIterator iterator;
public:
	inline CustomPartIter(const PartIterator& iter) : iterator(iter) {}
	inline CustomPartIter(const ListOfPtrIter<Physical>& start, const PartIter& partInPhys) : iterator(start, partInPhys) {}
	inline T& operator*() const {
		return static_cast<T&>(*iterator);
	}

	inline CustomPartIter& operator++() {
		++iterator;
		return *this;
	}

	inline bool operator!=(const CustomPartIter& other) { return this->iterator != other.iterator; }
};*/
template<typename T>
class ConstCustomPartIter {
	ConstPartIterator iterator;
public:
	inline ConstCustomPartIter(const ConstPartIterator& iter) : iterator(iter) {}
	inline ConstCustomPartIter(const Physical* current, ConstPartIter currentPartInPhys) : iterator(current, currentPartInPhys) {}
	inline const T& operator*() const {
		return static_cast<const T&>(*iterator);
	}

	inline ConstCustomPartIter& operator++() {
		++iterator;
		return *this;
	}

	inline bool operator!=(const ConstCustomPartIter& other) const { return this->iterator != other.iterator; }
};

/*template<typename T>
using CustomPartIteratorFactory = IteratorFactory<CustomPartIter<T>>*/

template<typename T>
class CustomPartIteratorFactory {
	ListOfPtrIterFactory<Physical> iterFactory;
public:
	inline CustomPartIteratorFactory(const ListOfPtrIterFactory<Physical>& iterFactory) : iterFactory(iterFactory) {}
	inline CustomPartIter<T> begin() const { return CustomPartIter<T>(iterFactory.begin()); }
	inline CustomPartIter<T> end() const { return CustomPartIter<T>(iterFactory.end()); }
};

template<typename T>
class ConstCustomPartIteratorFactory {
	ListOfPtrIterFactory<const Physical> iterFactory;
public:
	inline ConstCustomPartIteratorFactory(const ListOfPtrIterFactory<const Physical>& iterFactory) : iterFactory(iterFactory) {}
	inline ConstCustomPartIter<T> begin() const { return ConstCustomPartIter<T>(iterFactory.begin()); }
	inline ConstCustomPartIter<T> end() const { return ConstCustomPartIter<T>(iterFactory.end()); }
};
