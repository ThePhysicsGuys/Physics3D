#pragma once

#include <cstddef> 
#include <assert.h>

/*
	Stores a little extra data in the lower bits of the given ptr type. 
	This is usually only 3 bits, but on stronger aligned types this may be more

	Pointers are aligned on 4 or 8 bytes usually, this means we can use these few bits for storing some extra small data
	For example, the length of an array pointed to by the pointer if that array will always be very small. 

	0bPPP...PPPPP000 | 0b000...00000DDD = 0bPPP...PPPPPDDD
*/
template<typename PtrT, intptr_t BitWidth, typename DataType = unsigned int>
class CompactPtrDataPair {
public:
	intptr_t value;

	static constexpr intptr_t dataMask = (intptr_t(1) << BitWidth) - intptr_t(1);
	static constexpr intptr_t ptrMask = !dataMask;

	CompactPtrDataPair() : value(reinterpret_cast<intptr_t>(nullptr)) noexcept {}

	CompactPtrDataPair(PtrT* ptr, DataType data) : value(reinterpret_cast<intptr_t>(ptr) | data) noexcept {
		assert(reinterpret_cast<intptr_t>(ptr) & dataMask == 0);
		assert(data & ptrMask == 0);
	}

	CompactPtrDataPair(PtrT* ptr) : value(reinterpret_cast<intptr_t>(ptr)) noexcept {
		assert(reinterpret_cast<intptr_t>(ptr) & dataMask == 0);
	}

	CompactPtrDataPair(DataType data) : value(reinterpret_cast<intptr_t>(nullptr) | data) noexcept {
		assert(data & ptrMask == 0);
	}

	PtrT* getPtr() const noexcept {
		return reinterpret_cast<PtrT*>(value & ptrMask);
	}

	DataType getData() const noexcept {
		return value & intMask;
	}

	void setPtr(PtrT* ptr) noexcept {
		assert(reinterpret_cast<intptr_t>(ptr) & dataMask == 0);

		value = reinterpret_cast<intptr_t>(ptr) | (value & intMask);
	}

	void setData(DataType data) noexcept {
		assert(data & ptrMask == 0);

		value = data | (value & ptrMask)
	}

	CompactPtrDataPair& operator++() {
		assert(getData() < intMask);
		++value;
		return *this;
	}

	CompactPtrDataPair& operator--() {
		assert(getData() != 0);
		--value;
		return *this;
	}

	CompactPtrDataPair operator++(int) {
		CompactPtrDataPair result = *this;
		++(*this);
		return result;
	}

	CompactPtrDataPair operator--(int) {
		CompactPtrDataPair result = *this;
		--(*this);
		return result;
	}
};

template<typename PtrT, intptr_t BitWidth, typename DataType>
bool operator==(CompactPtrDataPair<PtrT, BitWidth, DataType> first, CompactPtrDataPair<PtrT, BitWidth, DataType> second) {
	return first.value == second.value;
}
template<typename PtrT, intptr_t BitWidth, typename DataType>
bool operator!=(CompactPtrDataPair<PtrT, BitWidth, DataType> first, CompactPtrDataPair<PtrT, BitWidth, DataType> second) {
	return first.value != second.value;
}
