#pragma once

#include <inttypes.h>

template<int64_t N>
struct Fix;

#define ONE (1ULL << N)

template<int64_t N>
struct Fix {
	int64_t value;

	constexpr Fix() noexcept : value(0) {}
	constexpr Fix(double d) noexcept : value(static_cast<int64_t>(d * ONE)) {}
	constexpr Fix(float f) noexcept : value(static_cast<int64_t>(double(f) * ONE)) {}
	constexpr explicit Fix(int64_t l) noexcept : value(l) {}

	inline constexpr operator double() const noexcept { return static_cast<double>(value) / ONE; }
	inline constexpr operator float() const noexcept { return static_cast<float>(value) / ONE; }

	inline Fix<N>& operator++() noexcept { value += ONE; return *this; }
	inline Fix<N> operator++(int) noexcept { Fix<N> old = *this; value += ONE; return old; }
	inline Fix<N>& operator--() noexcept { value -= ONE; return *this; }
	inline Fix<N> operator--(int) noexcept { Fix<N> old = *this; value -= ONE; return old; }

	inline Fix<N>& operator+=(const Fix<N>& b) noexcept { this->value += b.value; return *this; }
	inline Fix<N>& operator-=(const Fix<N>& b) noexcept { this->value -= b.value; return *this; }
	inline Fix<N>& operator+=(int64_t b) noexcept { this->value += (b << N); return *this; }
	inline Fix<N>& operator-=(int64_t b) noexcept { this->value -= (b << N); return *this; }

	static inline constexpr Fix<N> maxValue() noexcept {return Fix<N>(0x7FFFFFFFFFFFFFFF); }

	inline constexpr Fix<N> operator-() const noexcept { return Fix<N>(-this->value); }
};


template<int64_t N> inline constexpr Fix<N> operator+(Fix<N> a, Fix<N> b) noexcept { return Fix<N>(a.value + b.value); }
template<int64_t N> inline constexpr Fix<N> operator-(Fix<N> a, Fix<N> b) noexcept { return Fix<N>(a.value - b.value); }
template<int64_t N> inline constexpr Fix<N> operator+(Fix<N> a, int64_t b) noexcept { return Fix<N>(a.value + (b << N)); }
template<int64_t N> inline constexpr Fix<N> operator-(Fix<N> a, int64_t b) noexcept { return Fix<N>(a.value - (b << N)); }
template<int64_t N> inline constexpr Fix<N> operator+(int64_t a, Fix<N> b) noexcept { return Fix<N>((a << N) + b.value); }
template<int64_t N> inline constexpr Fix<N> operator-(int64_t a, Fix<N> b) noexcept {return Fix<N>((a << N) - b.value);}

template<int64_t N> inline constexpr Fix<N> operator+(Fix<N> a, double b) noexcept { return a + Fix<N>(b); }
template<int64_t N> inline constexpr Fix<N> operator+(double a, Fix<N> b) noexcept { return Fix<N>(a) + b; }
template<int64_t N> inline constexpr Fix<N> operator-(Fix<N> a, double b) noexcept { return a - Fix<N>(b); }
template<int64_t N> inline constexpr Fix<N> operator-(double a, Fix<N> b) noexcept { return Fix<N>(a) - b; }

template<int64_t N> inline constexpr Fix<N> operator*(Fix<N> a, double b) noexcept {return Fix<N>(static_cast<int64_t>(a.value * b));}
template<int64_t N> inline constexpr Fix<N> operator*(Fix<N> a, float b)  noexcept {return Fix<N>(static_cast<int64_t>(a.value * b));}
template<int64_t N> inline constexpr Fix<N> operator*(double a, Fix<N> b) noexcept {return Fix<N>(static_cast<int64_t>(a * b.value));}
template<int64_t N> inline constexpr Fix<N> operator*(float a, Fix<N> b)  noexcept {return Fix<N>(static_cast<int64_t>(a * b.value));}

template<int64_t N> inline constexpr Fix<N> operator/(Fix<N> a, double b) noexcept { return Fix<N>(static_cast<int64_t>(a.value / b)); }
template<int64_t N> inline constexpr Fix<N> operator/(Fix<N> a, float b)  noexcept { return Fix<N>(static_cast<int64_t>(a.value / b)); }
template<int64_t N> inline constexpr Fix<N> operator/(double a, Fix<N> b) noexcept { return Fix<N>(static_cast<int64_t>(a / b.value)); }
template<int64_t N> inline constexpr Fix<N> operator/(float a, Fix<N> b)  noexcept { return Fix<N>(static_cast<int64_t>(a / b.value)); }

template<int64_t N> inline constexpr Fix<N> operator/(Fix<N> a, int b) noexcept { return Fix<N>(a.value / b); }

template<int64_t N> inline constexpr Fix<N> operator*(Fix<N> a, int64_t b) noexcept { return Fix<N>(a.value * b); }
template<int64_t N> inline constexpr Fix<N> operator*(int64_t a, Fix<N> b) noexcept { return Fix<N>(a * b.value); }

template<int64_t N> inline constexpr Fix<N> operator/(Fix<N> a, int64_t b) noexcept { return Fix<N>(static_cast<int64_t>(a.value / b)); }

template<int64_t N> inline constexpr Fix<N> operator<<(Fix<N> a, int64_t b) noexcept { return Fix<N>(static_cast<int64_t>(a.value << b)); }
template<int64_t N> inline constexpr Fix<N> operator>>(Fix<N> a, int64_t b) noexcept { return Fix<N>(static_cast<int64_t>(a.value >> b)); }


#define CREATE_COMPARISONS(T1, T2, V1, V2) \
template<int64_t N> inline constexpr bool operator==(T1 a, T2 b) noexcept { return V1 == V2; } \
template<int64_t N> inline constexpr bool operator!=(T1 a, T2 b) noexcept { return V1 != V2; } \
template<int64_t N> inline constexpr bool operator>=(T1 a, T2 b) noexcept { return V1 >= V2; } \
template<int64_t N> inline constexpr bool operator<=(T1 a, T2 b) noexcept { return V1 <= V2; } \
template<int64_t N> inline constexpr bool operator>(T1 a, T2 b) noexcept { return V1 > V2; } \
template<int64_t N> inline constexpr bool operator<(T1 a, T2 b) noexcept { return V1 < V2; }

CREATE_COMPARISONS(Fix<N>, Fix<N>, a.value, b.value);
CREATE_COMPARISONS(Fix<N>, double, a, Fix<N>(b));
CREATE_COMPARISONS(double, Fix<N>, Fix<N>(a), b);
CREATE_COMPARISONS(Fix<N>, float, a, Fix<N>(b));
CREATE_COMPARISONS(float, Fix<N>, Fix<N>(a), b);
CREATE_COMPARISONS(Fix<N>, int64_t, a.value, b << N);
CREATE_COMPARISONS(int64_t, Fix<N>, a << N, b.value);
CREATE_COMPARISONS(Fix<N>, int, a.value, int64_t(b) << N);
CREATE_COMPARISONS(int, Fix<N>, int64_t(a) << N, b.value);
#undef CREATE_COMPARISONS

template<int N>
inline constexpr Fix<N> min(Fix<N> first, Fix<N> second) noexcept {
	return (first.value <= second.value)? first : second;
}

template<int N>
inline constexpr Fix<N> max(Fix<N> first, Fix<N> second) noexcept {
	return (first.value >= second.value) ? first : second;
}
#undef ONE

template<int64_t N>
Fix<N> quickMultiply(Fix<N> a, Fix<N> b) noexcept {
	return Fix<N>((a.value >> (N / 2)) * (b.value >> (N - N / 2)));
}
