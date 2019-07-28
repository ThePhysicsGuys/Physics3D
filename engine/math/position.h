#pragma once

#include "fix.h"
#include "vec3.h"

typedef Vec3Template<Fix<32>> Vec3Fix;

struct Position {
	union {
		struct {
			Fix<32> x;
			Fix<32> y;
			Fix<32> z;
		};
		Fix<32> v[3];
	};
	inline constexpr Position() : x(), y(), z() {}
	inline constexpr Position(Fix<32> x, Fix<32> y, Fix<32> z) : x(x), y(y), z(z) {}
	inline constexpr const Fix<32>& operator[](size_t index) const { return v[index]; }
	inline Fix<32>& operator[](size_t index) { return v[index]; }
};

inline Vec3Fix operator-(const Position& a, const Position& b) {
	return Vec3Fix(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline constexpr Position operator+(const Position& a, const Vec3Fix& b) {
	return Position(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline constexpr Position operator-(const Position& a, const Vec3Fix& b) {
	return Position(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline constexpr bool operator==(const Position& first, const Position& second) {return first.x == second.x && first.y == second.y && first.z == second.z;}
inline constexpr bool operator!=(const Position& first, const Position& second) { return first.x != second.x || first.y != second.y || first.z != second.z; }

inline constexpr Position max(const Position& p1, const Position& p2) {
	return Position(max(p1.x, p2.x), max(p1.y, p2.y), max(p1.z, p2.z));
}
inline constexpr Position min(const Position& p1, const Position& p2) {
	return Position(min(p1.x, p2.x), min(p1.y, p2.y), min(p1.z, p2.z));
}
inline Position avg(const Position& first, const Position& second) {
	Vec3Fix delta = second - first;
	return first + Vec3Fix(delta.x >> 1, delta.y >> 1, delta.z >> 1);
}

// unconventional operator, compares xyz individually, returns true if all are true
inline constexpr bool operator>=(const Position& first, const Position& second) { return first.x >= second.x && first.y >= second.y && first.z >= second.z; }
// unconventional operator, compares xyz individually, returns true if all are true
inline constexpr bool operator<=(const Position& first, const Position& second) { return first.x <= second.x && first.y <= second.y && first.z <= second.z; }
// unconventional operator, compares xyz individually, returns true if all are true
inline constexpr bool operator>(const Position& first, const Position& second) { return first.x > second.x && first.y > second.y && first.z > second.z; }
// unconventional operator, compares xyz individually, returns true if all are true
inline constexpr bool operator<(const Position& first, const Position& second) { return first.x < second.x&& first.y < second.y&& first.z < second.z; }
