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
namespace P3D {
template<typename PtrT, unsigned int BitWidth = 1, typename DataType = unsigned int>
class CompactPtrDataPair {
protected:
	std::uintptr_t value;

	CompactPtrDataPair(std::uintptr_t value) : value(value) {}

public:
	static constexpr std::uintptr_t dataMask = (std::uintptr_t(1) << BitWidth) - std::uintptr_t(1);
	static constexpr std::uintptr_t ptrMask = ~dataMask;

	CompactPtrDataPair() noexcept : value(reinterpret_cast<std::uintptr_t>(nullptr)) {}

	CompactPtrDataPair(PtrT* ptr, DataType data) noexcept : value(reinterpret_cast<std::uintptr_t>(ptr) | data) {
		assert(reinterpret_cast<std::uintptr_t>(ptr) & dataMask == std::uintptr_t(0));
		assert(data & ptrMask == std::uintptr_t(0));
	}

	PtrT* getPtr() const noexcept {
		return reinterpret_cast<PtrT*>(value & ptrMask);
	}

	bool isNullptr() const noexcept {
		return value & ptrMask == reinterpret_cast<std::uintptr_t>(nullptr) & ptrMask;
	}

	DataType getData() const noexcept {
		return value & dataMask;
	}

	void setPtr(PtrT* ptr) noexcept {
		assert(reinterpret_cast<std::uintptr_t>(ptr) & dataMask == std::uintptr_t(0));

		value = reinterpret_cast<std::uintptr_t>(ptr) | (value & dataMask);
	}

	void setData(DataType data) noexcept {
		assert(data & ptrMask == std::uintptr_t(0));

		value = data | (value & ptrMask)
	}

	CompactPtrDataPair& operator++() {
		assert(getData() < dataMask);
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

template<typename PtrT>
class CompactPtrDataPair<PtrT, 1, bool> {
protected:
	std::uintptr_t value;

	CompactPtrDataPair(std::uintptr_t value) : value(value) {}

public:
	static constexpr std::uintptr_t dataMask = std::uintptr_t(1);
	static constexpr std::uintptr_t ptrMask = ~dataMask;

	CompactPtrDataPair() noexcept : value(reinterpret_cast<std::uintptr_t>(nullptr)) {}

	CompactPtrDataPair(PtrT* ptr, bool data) noexcept : value(reinterpret_cast<std::uintptr_t>(ptr) | (data ? std::uintptr_t(1) : std::uintptr_t(0))) {
		assert(reinterpret_cast<std::uintptr_t>(ptr) & dataMask == std::uintptr_t(0));
	}

	PtrT* getPtr() const noexcept {
		return reinterpret_cast<PtrT*>(value & ptrMask);
	}

	bool isNullptr() const noexcept {
		return value & ptrMask == reinterpret_cast<std::uintptr_t>(nullptr) & ptrMask;
	}

	bool getData() const noexcept {
		return value & dataMask != std::uintptr_t(0);
	}

	void setPtr(PtrT* ptr) noexcept {
		assert(reinterpret_cast<std::uintptr_t>(ptr) & dataMask == std::uintptr_t(0));

		value = reinterpret_cast<std::uintptr_t>(ptr) | (value & dataMask);
	}

	void setData(bool data) noexcept {
		if(data) {
			value |= std::uintptr_t(1);
		} else {
			value &= ptrMask;
		}
	}

	void setDataTrue() noexcept {
		value |= std::uintptr_t(1);
	}

	void setDataFalse() noexcept {
		value &= ptrMask;
	}

	void invertData() noexcept {
		value ^= std::uintptr_t(1);
	}
};

template<typename PtrT, unsigned int BitWidth, typename DataType>
bool operator==(CompactPtrDataPair<PtrT, BitWidth, DataType> first, CompactPtrDataPair<PtrT, BitWidth, DataType> second) {
	return first.value == second.value;
}
template<typename PtrT, unsigned int BitWidth, typename DataType>
bool operator!=(CompactPtrDataPair<PtrT, BitWidth, DataType> first, CompactPtrDataPair<PtrT, BitWidth, DataType> second) {
	return first.value != second.value;
}

template<typename PtrT, unsigned int BitWidth = 1, typename DataType = unsigned int>
class CompactUniquePtrDataPair : public CompactPtrDataPair<PtrT, BitWidth, DataType> {
public:
	CompactUniquePtrDataPair() noexcept : CompactPtrDataPair() {}
	CompactUniquePtrDataPair(PtrT* ptr, DataType data) noexcept : CompactPtrDataPair(ptr, data) {}
	
	CompactUniquePtrDataPair(const CompactUniquePtrDataPair&) = delete;
	CompactUniquePtrDataPair& operator=(const CompactUniquePtrDataPair&) = delete;

	CompactUniquePtrDataPair(CompactUniquePtrDataPair&& other) noexcept : CompactPtrDataPair(other.value) {
		other.value = reinterpret_cast<std::uintptr_t>(nullptr);
	}
	CompactUniquePtrDataPair& operator=(CompactUniquePtrDataPair&& other) noexcept {
		std::uintptr_t tmpValue = this->value;
		this->value = other.value;
		other.value = tmpValue;
	}

	~CompactPtrDataPair() {
		if(!this->isNullptr()) delete this->getPtr();
	}
};

template<typename PtrT>
using CompactPtrBoolPair = CompactPtrDataPair<PtrT, 1, bool>;
template<typename PtrT>
using CompactUniquePtrBoolPair = CompactUniquePtrDataPair<PtrT, 1, bool>;

}