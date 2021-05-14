#pragma once
#include <utility>
#include "iteratorEnd.h"

namespace P3D {
template<typename BeginType, typename EndType = BeginType>
class IteratorFactory {
	BeginType start;
	EndType fin;
public:
	IteratorFactory() = default;
	IteratorFactory(const BeginType& start, const EndType& fin) : start(start), fin(fin) {}
	IteratorFactory(BeginType&& start, EndType&& fin) : start(std::move(start)), fin(std::move(fin)) {}

	BeginType begin() const { return start; }
	EndType end() const { return fin; }
};

template<typename BeginType>
class IteratorFactoryWithEnd {
	BeginType iter;
public:
	IteratorFactoryWithEnd() = default;
	IteratorFactoryWithEnd(const BeginType& iter) : iter(iter) {}
	IteratorFactoryWithEnd(BeginType&& iter) : iter(std::move(iter)) {}

	BeginType begin() const { return iter; }
	IteratorEnd end() const { return IteratorEnd(); }
};
};