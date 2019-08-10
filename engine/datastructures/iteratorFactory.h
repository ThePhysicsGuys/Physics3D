#pragma once

template<typename BeginType, typename EndType = BeginType>
class IteratorFactory {
	BeginType start;
	EndType fin;
public:
	IteratorFactory(const BeginType& start, const EndType& fin) : start(start), fin(fin) {}
	IteratorFactory(BeginType&& start, EndType&& fin) : 
		start(std::move(start)), fin(std::move(fin)) {
	
	}

	BeginType begin() const { return start; }
	EndType end() const { return fin; }
};

template<typename BaseIteratorFactoryType, typename NewBegin, typename NewEnd = NewBegin>
class CastingIteratorFactory {
	BaseIteratorFactoryType factory;
public:
	NewBegin begin() const { return static_cast<NewBegin>(factory.begin()); }
	NewEnd end() const { return static_cast<NewEnd>(factory.end()); }
};
