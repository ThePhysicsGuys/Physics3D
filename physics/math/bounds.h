#pragma once

#include "position.h"
#include "boundingBox.h"

template<typename T>
struct BoundsTemplate {
	PositionTemplate<T> min;
	PositionTemplate<T> max;

	constexpr BoundsTemplate() = default;
	constexpr BoundsTemplate(const PositionTemplate<T>& min, const PositionTemplate<T>& max) noexcept : min(min), max(max) {}

	constexpr Vector<T, 3> getDiagonal() const noexcept {
		return max - min;
	}

	constexpr bool contains(const PositionTemplate<T>& p) const noexcept {
		return p >= min && p <= max;
	}

	constexpr bool contains(const BoundsTemplate& other) const noexcept {
		return other.min >= this->min && other.max <= this->max;
	}

	constexpr PositionTemplate<T> getCenter() const noexcept {
		return avg(min, max);
	}

	constexpr BoundsTemplate expanded(T amount) const noexcept {
		return BoundsTemplate(min - Vector<T, 3>(amount, amount, amount), max + Vector<T, 3>(amount, amount, amount));
	}

	constexpr BoundsTemplate expanded(Vector<T, 3> amount) const noexcept {
		return BoundsTemplate(min - amount, max + amount);
	}

	constexpr T getWidth() const noexcept { return max.x - min.x; }
	constexpr T getHeight() const noexcept { return max.y - min.y; }
	constexpr T getDepth() const noexcept { return max.z - min.z; }
	constexpr T getVolume() const noexcept { Vector<T, 3> diag = max - min; return diag.x * diag.y * diag.z; }
};

template<typename T>
constexpr bool intersects(const BoundsTemplate<T>& first, const BoundsTemplate<T>& second) noexcept {
	return first.max >= second.min && first.min <= second.max;
}
template<typename T>
constexpr BoundsTemplate<T> unionOfBounds(const BoundsTemplate<T>& first, const BoundsTemplate<T>& second) noexcept {
	return BoundsTemplate<T>(min(first.min, second.min), max(first.max, second.max));
}
template<typename T>
constexpr bool operator==(const BoundsTemplate<T>& first, const BoundsTemplate<T>& second) noexcept {
	return first.min == second.min && first.max == second.max;
}
template<typename T>
constexpr bool operator!=(const BoundsTemplate<T>& first, const BoundsTemplate<T>& second) noexcept {
	return first.min != second.min || first.max != second.max;
}

template<typename PT, typename VT>
constexpr BoundsTemplate<PT> operator+(const BoundingBoxTemplate<VT>& localBox, const PositionTemplate<PT>& pos) noexcept {
	PositionTemplate<PT> min = pos + localBox.min;
	PositionTemplate<PT> max = pos + localBox.max;
	return BoundsTemplate<PT>(min, max);
}
template<typename T>
constexpr BoundingBoxTemplate<T> operator-(const BoundsTemplate<T>& box, const PositionTemplate<T>& pos) noexcept {
	Vector<T, 3> min = box.min - pos;
	Vector<T, 3> max = box.max - pos;
	return BoundingBoxTemplate<T>(min, max);
}

typedef BoundsTemplate<Fix<32>> Bounds;
