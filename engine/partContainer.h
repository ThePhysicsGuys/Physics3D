#pragma once

#include "physical.h"
#include "part.h"
#include <mutex>
#include <shared_mutex>

class ConstPartIterator {
	const Part* const * current;

public:
	inline ConstPartIterator(const Part* const * current) : current(current) {}
	inline const Part& operator*() const {
		return **current;
	}

	inline ConstPartIterator& operator++() {
		current++;
		return *this;
	}

	inline bool operator!=(ConstPartIterator& other) { return this->current != other.current; }
	inline bool operator==(ConstPartIterator& other) { return this->current == other.current; }
};

class PartIterator {
	Part** current;

public:
	inline PartIterator(Part** current) : current(current) {}
	inline Part& operator*() const {
		return **current;
	}

	inline PartIterator& operator++() {
		current++;
		return *this;
	}

	inline bool operator!=(PartIterator& other) { return this->current != other.current; }
	inline bool operator==(PartIterator& other) { return this->current == other.current; }
};

class ConstPartIteratorFactory {
	const Part* const * start;
	const Part* const * finish;
public:
	inline ConstPartIteratorFactory(const Part* const * start, const Part* const * finish) : start(start), finish(finish) {}
	inline ConstPartIterator begin() const { return ConstPartIterator(start); }
	inline ConstPartIterator end() const { return ConstPartIterator(finish); }
};

class PartIteratorFactory {
	Part** start;
	Part** finish;
public:
	inline PartIteratorFactory(Part** start, Part** finish) : start(start), finish(finish) {}
	inline PartIterator begin() const { return PartIterator(start); }
	inline PartIterator end() const { return PartIterator(finish); }
};

class PartContainer {
private:
	size_t capacity;
public:
	size_t physicalCount = 0;
	size_t freePhysicalsOffset = 0;
	size_t partCount = 0;
	size_t anchoredPartsCount = 0;
	Physical* physicals;
	Part** parts;

private:

	void movePhysical(size_t origin, size_t destination);
	void movePhysical(Physical* origin, Physical* destination);

	void swapPhysical(size_t first, size_t second);
	void swapPhysical(Physical* first, Physical* second);

	void movePart(size_t origin, size_t destination);
	//void movePart(Part** origin, Part** destination);

	void swapPart(size_t first, size_t second);
	void swapPart(Part** first, Part** second);

public:
	PartContainer(size_t initialCapacity);
	void ensureCapacity(size_t capacity);
	void remove(Part* p);
	void anchor(size_t index);
	void anchor(Physical* p);
	void unanchor(size_t index);
	void unanchor(Physical* p);
	bool isAnchored(size_t index) const;
	bool isAnchored(const Physical* index) const;
	bool isAnchored(const Part* const * index) const;

	void add(Part* part, bool anchored);
	
	template<typename T>
	struct Iter {
		T* first; T* last;
		T* begin() { return first; }
		T* end() { return last; }
	};
	template<typename T>
	struct ConstIter {
		const T* first; const T* last;
		const T* begin() const { return first; }
		const T* end() const { return last; }
	};

	Iter<Physical> iterPhysicals() { return Iter<Physical>{physicals, physicals + physicalCount}; }
	ConstIter<Physical> iterPhysicals() const { return ConstIter<Physical>{physicals, physicals + physicalCount}; }

	Iter<Physical> iterAnchoredPhysicals() { return Iter<Physical>{physicals, physicals + freePhysicalsOffset}; }
	ConstIter<Physical> iterAnchoredPhysicals() const { return ConstIter<Physical>{physicals, physicals + freePhysicalsOffset}; }

	Iter<Physical> iterUnAnchoredPhysicals() { return Iter<Physical>{physicals + freePhysicalsOffset, physicals + physicalCount}; }
	ConstIter<Physical> iterUnAnchoredPhysicals() const { return ConstIter<Physical>{physicals + freePhysicalsOffset, physicals + physicalCount}; }


	inline PartIterator begin() { return PartIterator(parts); }
	inline PartIterator end() { return PartIterator(parts + partCount); }

	inline ConstPartIterator begin() const { return ConstPartIterator(parts); }
	inline ConstPartIterator end() const { return ConstPartIterator(parts + partCount); }

	inline PartIteratorFactory iterAnchoredParts() { return PartIteratorFactory(parts, parts + anchoredPartsCount); }
	inline ConstPartIteratorFactory iterAnchoredParts() const { return ConstPartIteratorFactory(parts, parts + anchoredPartsCount); }

	inline PartIteratorFactory iterFreeParts() { return PartIteratorFactory(parts + anchoredPartsCount, parts + partCount); }
	inline ConstPartIteratorFactory iterFreeParts() const { return ConstPartIteratorFactory(parts + anchoredPartsCount, parts + partCount); }
};

