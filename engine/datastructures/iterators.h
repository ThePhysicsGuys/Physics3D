#pragma once

#include "iteratorEnd.h"

/*
	An iterator that can iterator over a group of iterables
	For example, to iterate over all parts in all the physicals would be
	CompositeIterator<Iterator<Physical>>
	These can be stacked, for example, to iterate over all the parts in all the physicals of all the worlds use
	CompositeIterator<CompositeIterator<Iterator<World>>>
*/
template<typename GroupIterator>
class CompositeIterator {
public:
	GroupIterator currentGroup;
	//decltype(*std::declval<GroupIterator>()) testItem;
	decltype((*std::declval<GroupIterator>()).begin()) currentItem;

	CompositeIterator() = default;
	CompositeIterator(const GroupIterator& groupIter) : currentGroup(groupIter), currentItem((*this->currentGroup).begin()) {}
	CompositeIterator(const GroupIterator& groupIter, const decltype((*std::declval<GroupIterator>()).begin()) currentItem) : currentGroup(groupIter), currentItem(currentItem) {}

	decltype(*currentItem)& operator*() const {
		return *currentItem;
	}

	inline CompositeIterator<GroupIterator>& operator++() {
		++currentItem;
		return *this;
	}

	// A bit awkward but this was the only way I could come up with so that it wouldn't run off the end of the group list
	inline bool operator!=(const CompositeIterator<GroupIterator>& other) {
		if (currentItem != (*currentGroup).end()) {
			return true;
		} else {
			++currentGroup;
			if (currentGroup != other.currentGroup) {
				currentItem = (*currentGroup).begin();
				return true;
			} else {
				return false;
			}
		}
	}
};

template<typename BaseIterator, typename OutputType>
class CastingIterator {
	BaseIterator iter;
public:
	CastingIterator() = default;
	CastingIterator(const BaseIterator& iter) : iter(iter) {}

	OutputType operator*() const {
		return static_cast<OutputType>(*iter);
	}
	CastingIterator<BaseIterator, OutputType>& operator++() {
		++iter;
		return *this;
	}
	bool operator!=(CastingIterator<BaseIterator, OutputType>& other) {
		return iter != other.iter;
	}
	bool operator!=(BaseIterator& other) {
		return iter != other;
	}
};

template<typename Iter, typename IterEnd, typename Filter, bool (*filterFunc)(const decltype(*std::declval<Iter>())& obj, const Filter& filter)>
class FilteredIterator {
	Iter iter;
	IterEnd iterEnd;
	Filter filter;
	
public:
	FilteredIterator(const Iter& iter, const IterEnd& iterEnd, const Filter& filter) : iter(iter), filter(filter) {
		while (!filter(*iter, filter) && !(iter != iterEnd)) {
			++iter;
		}
	}
	void operator++() {
		do {
			++iter;
		} while (!filter(*iter, filter) && !(iter != iterEnd));
	}
	decltype(*std::declval<Iter>()) operator*() const {
		return *iter;
	}
	bool operator!=(IteratorEnd) const {
		return iter != iterEnd;
	}
};
