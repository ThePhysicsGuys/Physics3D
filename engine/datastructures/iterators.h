#pragma once

#include "iteratorEnd.h"

/*
	An iterator that can iterator over a group of iterables
	For example, to iterate over all parts in all the physicals would be
	CompositeIterator<Iterator<Physical>>
	These can be stacked, for example, to iterate over all the parts in all the physicals of all the worlds use
	CompositeIterator<CompositeIterator<Iterator<World>>>
*/
template<typename GroupIterator, typename GroupIteratorEnd = GroupIterator>
class CompositeIterator {
public:
	GroupIterator currentGroup;
	GroupIteratorEnd groupIterEnd;
	decltype((*std::declval<GroupIterator>()).begin()) currentItem;

	CompositeIterator() = default;
	CompositeIterator(const GroupIterator& groupIter, const GroupIteratorEnd& groupIterEnd) : currentGroup(groupIter), currentItem((*this->currentGroup).begin()), groupIterEnd(groupIterEnd) {}
	CompositeIterator(const GroupIterator& groupIter, const decltype((*std::declval<GroupIterator>()).begin()) currentItem, const GroupIteratorEnd& groupIterEnd) : currentGroup(groupIter), currentItem(currentItem), groupIterEnd(groupIterEnd) {}

	decltype(*currentItem)& operator*() const {
		return *currentItem;
	}

	inline CompositeIterator<GroupIterator>& operator++() {
		++currentItem;
		if (currentItem == (*currentGroup).end()) {
			++currentGroup;
			if (currentGroup != groupIterEnd) {
				currentItem = (*currentGroup).begin();
			}
		}
		return *this;
	}

	inline bool operator!=(IteratorEnd) const {
		return currentGroup != groupIterEnd;
	}
};

template<typename BaseIterator, typename OutputType>
class CastingIterator {
	BaseIterator iter;
public:
	CastingIterator() = default;
	CastingIterator(const BaseIterator& iter) : iter(iter) {}
	/*template<typename BaseIteratorConstructor>
	CastingIterator(const BaseIteratorConstructor& iter) : iter(iter) {}*/

	OutputType operator*() const {
		return static_cast<OutputType>(*iter);
	}
	CastingIterator<BaseIterator, OutputType>& operator++() {
		++iter;
		return *this;
	}
	bool operator!=(const CastingIterator<BaseIterator, OutputType>& other) const {
		return iter != other.iter;
	}
	template<typename OtherIter>
	bool operator!=(const OtherIter& other) const {
		return iter != other;
	}
};

template<typename BaseIteratorFactoryType, typename NewType>
class CastingIteratorFactoryWithEnd {
	BaseIteratorFactoryType factory;
public:
	CastingIteratorFactoryWithEnd() = default;
	CastingIteratorFactoryWithEnd(const BaseIteratorFactoryType& factory) : factory(factory) {}

	CastingIterator<decltype(factory.begin()), NewType> begin() const { return CastingIterator<decltype(factory.begin()), NewType>(factory.begin()); }
	IteratorEnd end() const { return IteratorEnd(); }
};

template<typename BaseIteratorFactoryType, typename NewType, typename NewType2 = NewType>
class CastingIteratorFactory {
	BaseIteratorFactoryType factory;
public:
	CastingIteratorFactory() = default;
	CastingIteratorFactory(const BaseIteratorFactoryType& factory) : factory(factory) {}

	CastingIterator<decltype(factory.begin()), NewType> begin() const { return CastingIterator<decltype(factory.begin()), NewType>(factory.begin()); }
	CastingIterator<decltype(factory.end()), NewType2> end() const { return CastingIterator<decltype(factory.end()), NewType2>(factory.end()); }
};

template<typename Iter, typename IterEnd, typename Filter>
class FilteredIterator {
	Iter iter;
	IterEnd iterEnd;
	Filter filter;
	
public:
	FilteredIterator(const Iter& iter, const IterEnd& iterEnd, const Filter& filter) : iter(iter), filter(filter) {
		while (this->iter != this->iterEnd && !this->filter(*this->iter)) {
			++this->iter;
		}
	}
	void operator++() {
		do {
			++iter;
		} while (iter != iterEnd && !filter(*iter));
	}
	decltype(*std::declval<Iter>())& operator*() const {
		return *iter;
	}
	bool operator!=(IteratorEnd) const {
		return iter != iterEnd;
	}
};
