#pragma once

#include "iteratorEnd.h"

/*
	An iterator that can iterator over a group of iterables
	For example, to iterate over all parts in all the physicals would be
	CompositeIterator<Iterator<Physical>>
	These can be stacked, for example, to iterate over all the parts in all the physicals of all the worlds use
	CompositeIterator<CompositeIterator<Iterator<World>>>
*/
namespace P3D {
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
		if(currentItem == (*currentGroup).end()) {
			++currentGroup;
			if(currentGroup != groupIterEnd) {
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
public:
	Iter iter;
	IterEnd iterEnd;
	Filter filter;

	FilteredIterator(const Iter& iter, const IterEnd& iterEnd, const Filter& filter) : iter(iter), filter(filter) {
		while(this->iter != this->iterEnd && !this->filter(*this->iter)) {
			++this->iter;
		}
	}
	void operator++() {
		do {
			++iter;
		} while(iter != iterEnd && !filter(*iter));
	}
	decltype(*std::declval<Iter>())& operator*() const {
		return *iter;
	}
	bool operator!=(IteratorEnd) const {
		return iter != iterEnd;
	}
};

template<typename IterFactory, size_t BufferSize>
class IteratorGroup {
	IterFactory factories[BufferSize];
	size_t size = 0;

	decltype(std::declval<IterFactory>().begin()) curIter;
	decltype(std::declval<IterFactory>().end())  curEnd;

	size_t curFactoryIndex = 0;
public:

	IteratorGroup() = default;

	IteratorGroup(IterFactory(&list)[BufferSize], size_t count) : size(count), curIter(list[0].begin()), curEnd(list[0].end()), factories{} {
		if(count > BufferSize) throw "Invalid count!";
		for(size_t i = 0; i < count; i++) {
			factories[i] = list[i];
		}

		while(!(curIter != curEnd)) {
			++curFactoryIndex;
			if(curFactoryIndex == size) break;
			curIter = factories[curFactoryIndex].begin();
			curEnd = factories[curFactoryIndex].end();
		}
	}

	void operator++() {
		++curIter;
		while(!(curIter != curEnd)) {
			++curFactoryIndex;
			if(curFactoryIndex == size) return;
			curIter = factories[curFactoryIndex].begin();
			curEnd = factories[curFactoryIndex].end();
		}
	}
	decltype(*std::declval<IterFactory>().begin())& operator*() const {
		return *curIter;
	}
	bool operator!=(IteratorEnd) const {
		return curFactoryIndex != size;
	}
};

template<typename Iter>
struct DereferencingIterator {
	Iter baseIter;

	DereferencingIterator(const Iter& baseIter) : baseIter(baseIter) {}
	DereferencingIterator(Iter&& baseIter) : baseIter(std::move(baseIter)) {}

	void operator++() { ++baseIter; }
	void operator++(int) { baseIter++; }
	decltype(**baseIter)& operator*() const { return **baseIter; }
	bool operator!=(const DereferencingIterator& other) const { return this->baseIter != other.baseIter; }
	bool operator!=(const Iter& other) const { return this->baseIter != other; }
	bool operator==(const DereferencingIterator& other) const { return this->baseIter == other.baseIter; }
	bool operator==(const Iter& other) const { return this->baseIter == other; }
};

// utility for constructing both const and nonconst iterator simultaneously

template<typename Type, bool IsConst>
struct const_if_tmp {
	using type = Type;
};

template<typename Type>
struct const_if_tmp<Type, true> {
	using type = const Type;
};

template<typename Type, bool IsConst>
using const_if = typename const_if_tmp<Type, IsConst>::type;
};