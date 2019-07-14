#pragma once

#include "position.h"

struct Bounds {
	Position min;
	Position max;

	inline Bounds() = default;
	inline Bounds(const Position& min, const Position& max) : min(min), max(max) {}

	inline Vec3Fix getDiagonal() const {
		return max - min;
	}

	inline bool contains(const Position& p) const {
		return p >= min && p <= max;
	}

	inline bool contains(const Bounds& other) const {
		return other.min >= this->min && other.max <= this->max;
	}

	inline Position getCenter() const {
		return avg(min, max);
	}

	inline Bounds expanded(Fix<32> amount) const {
		return Bounds(min - Vec3Fix(amount, amount, amount), max + Vec3Fix(amount, amount, amount));
	}

	inline Bounds expanded(Vec3Fix amount) const {
		return Bounds(min - amount, max + amount);
	}
};

inline bool intersects(const Bounds& first, const Bounds& second) {
	return first.max >= second.min && first.min <= second.max;
}
inline Bounds unionOfBounds(const Bounds& first, const Bounds& second) {
	return Bounds(min(first.min, second.min), max(first.max, second.max));
}
