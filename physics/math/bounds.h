#pragma once

#include "position.h"
#include "../geometry/boundingBox.h"

struct Bounds {
	Position min;
	Position max;

	inline Bounds() = default;
	inline Bounds(const Position& min, const Position& max) noexcept : min(min), max(max) {}

	inline Vec3Fix getDiagonal() const noexcept {
		return max - min;
	}

	inline bool contains(const Position& p) const noexcept {
		return p >= min && p <= max;
	}

	inline bool contains(const Bounds& other) const noexcept {
		return other.min >= this->min && other.max <= this->max;
	}

	inline Position getCenter() const noexcept {
		return avg(min, max);
	}

	inline Bounds expanded(Fix<32> amount) const noexcept {
		return Bounds(min - Vec3Fix(amount, amount, amount), max + Vec3Fix(amount, amount, amount));
	}

	inline Bounds expanded(Vec3Fix amount) const noexcept {
		return Bounds(min - amount, max + amount);
	}

	inline Fix<32> getWidth()  const noexcept { return max.x - min.x; }
	inline Fix<32> getHeight() const noexcept { return max.y - min.y; }
	inline Fix<32> getDepth()  const noexcept { return max.z - min.z; }
};

inline bool intersects(const Bounds& first, const Bounds& second) noexcept {
	return first.max >= second.min && first.min <= second.max;
}
inline Bounds unionOfBounds(const Bounds& first, const Bounds& second) noexcept {
	return Bounds(min(first.min, second.min), max(first.max, second.max));
}
inline bool operator==(const Bounds& first, const Bounds& second) noexcept {
	return first.min == second.min && first.max == second.max;
}
inline bool operator!=(const Bounds& first, const Bounds& second) noexcept {
	return first.min != second.min || first.max != second.max;
}

inline Bounds operator+(const BoundingBox& localBox, const Position& pos) noexcept {
	Position min = pos + localBox.min;
	Position max = pos + localBox.max;
	return Bounds(min, max);
}
inline BoundingBox operator-(const Bounds& box, const Position& pos) noexcept {
	Vec3 min = box.min - pos;
	Vec3 max = box.max - pos;
	return BoundingBox(min, max);
}