#pragma once

#include "fix.h"
#include "linalg/vec.h"

typedef Vector<Fix<32>, 3> Vec3Fix;

template<typename T>
struct PositionTemplate {
	T x;
	T y;
	T z;
	constexpr PositionTemplate() noexcept : x(), y(), z() {}
	constexpr PositionTemplate(const PositionTemplate&) = default;
	constexpr PositionTemplate& operator=(const PositionTemplate&) = default;
	template<typename OtherT>
	constexpr PositionTemplate(const PositionTemplate<OtherT>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)) {}
	template<typename OtherT>
	constexpr PositionTemplate& operator=(const PositionTemplate<OtherT>& other) {
		this->x = static_cast<T>(other.x);
		this->y = static_cast<T>(other.y);
		this->z = static_cast<T>(other.z);
		return *this;
	}
	constexpr PositionTemplate(T x, T y, T z) noexcept : x(x), y(y), z(z) {}
	constexpr PositionTemplate(double x, double y, double z) noexcept : x(T(x)), y(T(y)), z(T(z)) {}
};
typedef PositionTemplate<Fix<32>> Position;

template<typename T>
constexpr Vector<T, 3> operator-(const PositionTemplate<T>& a, const PositionTemplate<T>& b) noexcept {
	return Vector<T, 3>(a.x - b.x, a.y - b.y, a.z - b.z);
}
template<typename PT, typename VT>
constexpr PositionTemplate<PT> operator+(const PositionTemplate<PT>& a, const Vector<VT, 3>& b) noexcept {
	return PositionTemplate<PT>(a.x + b.x, a.y + b.y, a.z + b.z);
}
template<typename PT, typename VT>
constexpr PositionTemplate<PT> operator-(const PositionTemplate<PT>& a, const Vector<VT, 3>& b) noexcept {
	return PositionTemplate<PT>(a.x - b.x, a.y - b.y, a.z - b.z);
}
template<typename PT, typename VT>
constexpr void operator+=(PositionTemplate<PT>& pos, const Vector<VT, 3>& offset) noexcept {
//inline void operator+=(Position& pos, const Vec3Fix& offset) noexcept {
	//int64_t tester = offset.x.value;
	pos.x += offset.x;
	pos.y += offset.y;
	pos.z += offset.z;
}
template<typename PT, typename VT>
constexpr void operator-=(PositionTemplate<PT>& pos, const Vector<VT, 3>& offset) noexcept {
//inline void operator-=(Position& pos, const Vec3Fix& offset) noexcept {
	//int64_t tester = offset.x.value;
	pos.x -= offset.x;
	pos.y -= offset.y;
	pos.z -= offset.z;
}

template<typename T>
constexpr bool operator==(const PositionTemplate<T>& first, const PositionTemplate<T>& second) noexcept {
	return first.x == second.x && first.y == second.y && first.z == second.z;
}
template<typename T>
constexpr bool operator!=(const PositionTemplate<T>& first, const PositionTemplate<T>& second) noexcept {
	return first.x != second.x || first.y != second.y || first.z != second.z;
}

template<typename T>
constexpr PositionTemplate<T> max(const PositionTemplate<T>& p1, const PositionTemplate<T>& p2) noexcept {
	return PositionTemplate<T>(p1.x > p2.x ? p1.x : p2.x, p1.y > p2.y ? p1.y : p2.y, p1.z > p2.z ? p1.z : p2.z);
}
template<typename T>
constexpr PositionTemplate<T> min(const PositionTemplate<T>& p1, const PositionTemplate<T>& p2) noexcept {
	return PositionTemplate<T>(p1.x < p2.x ? p1.x : p2.x, p1.y < p2.y ? p1.y : p2.y, p1.z < p2.z ? p1.z : p2.z);
}
template<typename T>
constexpr PositionTemplate<T> avg(const PositionTemplate<T>& first, const PositionTemplate<T>& second) noexcept {
	return PositionTemplate<T>((first.x + second.x) / 2, (first.y + second.y) / 2, (first.z + second.z) / 2);
}

// unconventional operator, compares xyz individually, returns true if all are true
template<typename T>
constexpr bool operator>=(const PositionTemplate<T>& first, const PositionTemplate<T>& second) noexcept { return first.x >= second.x && first.y >= second.y && first.z >= second.z; }
// unconventional operator, compares xyz individually, returns true if all are true
template<typename T>
constexpr bool operator<=(const PositionTemplate<T>& first, const PositionTemplate<T>& second) noexcept { return first.x <= second.x && first.y <= second.y && first.z <= second.z; }
// unconventional operator, compares xyz individually, returns true if all are true
template<typename T>
constexpr bool operator>(const PositionTemplate<T>& first, const PositionTemplate<T>& second) noexcept { return first.x > second.x && first.y > second.y && first.z > second.z; }
// unconventional operator, compares xyz individually, returns true if all are true
template<typename T>
constexpr bool operator<(const PositionTemplate<T>& first, const PositionTemplate<T>& second) noexcept { return first.x < second.x && first.y < second.y && first.z < second.z; }


// cast Vec3 to Position, not recommended due to potential loss of precision
constexpr Position castVec3ToPosition(const Vec3& vec) noexcept {
	return Position(vec.x, vec.y, vec.z);
}
// cast Vec3f to Position, not recommended due to potential loss of precision
constexpr Position castVec3fToPosition(const Vec3f& vec) noexcept {
	return Position(vec.x, vec.y, vec.z);
}
// cast Position to Vec3, not recommended due to potential loss of precision
// If possible, compute relative positions, eg: pos1-pos2 = vec3 delta
constexpr Vec3 castPositionToVec3(const Position& pos) noexcept {
	return Vec3(static_cast<double>(pos.x), static_cast<double>(pos.y), static_cast<double>(pos.z));
}
// cast Position to Vec3f, not recommended due to potential loss of precision
// If possible, compute relative positions, eg: pos1-pos2 = vec3f delta
constexpr Vec3f castPositionToVec3f(const Position& pos) noexcept {
	return Vec3f(static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z));
}
