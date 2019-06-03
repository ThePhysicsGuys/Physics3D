#pragma once

#include "part.h"
#include "physical.h"

class ConstPartIterator {
	const Physical* currentPhys;
	ConstPartIter currentPartInPhys;
public:
	inline ConstPartIterator(const Physical* currentPhys) : currentPhys(currentPhys), currentPartInPhys{ currentPhys->begin() } {}
	inline ConstPartIterator(const Physical* currentPhys, ConstPartIter currentPartInPhys) : currentPhys(currentPhys), currentPartInPhys(currentPartInPhys) {}
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

	inline bool operator!=(ConstPartIterator& other) { return this->currentPhys != other.currentPhys; }
};

class PartIterator {
	Physical* currentPhys;
	PartIter currentPartInPhys;
public:
	inline PartIterator(Physical* currentPhys) : currentPhys(currentPhys), currentPartInPhys{ currentPhys->begin() } {}
	inline PartIterator(Physical* currentPhys, PartIter currentPartInPhys) : currentPhys(currentPhys), currentPartInPhys(currentPartInPhys) {}
	inline Part& operator*() const {
		return *currentPartInPhys;
	}

	inline PartIterator& operator++() {
		++currentPartInPhys;
		if (currentPartInPhys == currentPhys->end()) {
			currentPhys++;
			currentPartInPhys = currentPhys->begin();
		}
		return *this;
	}

	inline bool operator!=(PartIterator& other) { return this->currentPhys != other.currentPhys; }
};

class ConstPartIteratorFactory {
	const Physical* start;
	const Physical* finish;
public:
	inline ConstPartIteratorFactory(const Physical* start, const Physical* finish) : start(start), finish(finish) {}
	inline ConstPartIterator begin() const { return ConstPartIterator(start, start->begin()); }
	inline ConstPartIterator end() const { return ConstPartIterator(finish, finish->end()); }
};

class PartIteratorFactory {
	Physical* start;
	Physical* finish;
public:
	inline PartIteratorFactory(Physical* start, Physical* finish) : start(start), finish(finish) {}
	inline PartIterator begin() const { return PartIterator(start, start->begin()); }
	inline PartIterator end() const { return PartIterator(finish, finish->end()); }
};


template<typename T>
class CustomPartIter {
	PartIterator iterator;
public:
	inline CustomPartIter(const PartIterator& iter) : iterator(iter) {}
	inline CustomPartIter(Physical* current, PartIter currentPartInPhys) : iterator(current, currentPartInPhys) {}
	inline T& operator*() const {
		return static_cast<T&>(*iterator);
	}

	inline CustomPartIter& operator++() {
		++iterator;
		return *this;
	}

	inline bool operator!=(CustomPartIter& other) { return this->iterator != other.iterator; }
	inline bool operator==(CustomPartIter& other) { return this->iterator == other.iterator; }
};
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

	inline bool operator!=(ConstCustomPartIter& other) { return this->iterator != other.iterator; }
	inline bool operator==(ConstCustomPartIter& other) { return this->iterator == other.iterator; }
};

template<typename T>
class CustomPartIteratorFactory {
	Physical* start;
	Physical* finish;
public:
	inline CustomPartIteratorFactory(const ListIter<Physical>& iter) : CustomPartIteratorFactory(iter.begin(), iter.end()) {}
	inline CustomPartIteratorFactory(Physical* start, Physical* finish) : start(start), finish(finish) {}
	inline CustomPartIter<T> begin() const { return CustomPartIter<T>(start, start->begin()); }
	inline CustomPartIter<T> end() const { return CustomPartIter<T>(finish, finish->end()); }
};

template<typename T>
class ConstCustomPartIteratorFactory {
	const Physical* start;
	const Physical* finish;
public:
	inline ConstCustomPartIteratorFactory(const ConstListIter<Physical>& iter) : ConstCustomPartIteratorFactory(iter.begin(), iter.end()) {}
	inline ConstCustomPartIteratorFactory(const Physical* start, const Physical* finish) : start(start), finish(finish) {}
	inline ConstCustomPartIter<T> begin() const { return ConstCustomPartIter<T>(start, start->begin()); }
	inline ConstCustomPartIter<T> end() const { return ConstCustomPartIter<T>(finish, finish->end()); }
};
