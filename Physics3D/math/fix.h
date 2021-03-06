#pragma once

#include <stdint.h>

namespace P3D {
template<int64_t N>
struct Fix {
	int64_t value;

	static constexpr int64_t ONE = int64_t(1) << N;

	constexpr Fix() noexcept : value(0) {}
	constexpr Fix(double d) noexcept : value(static_cast<int64_t>(d * ONE)) {}
	constexpr Fix(float f) noexcept : value(static_cast<int64_t>(static_cast<double>(f) * ONE)) {}
	constexpr Fix(int l) noexcept : value(static_cast<int64_t>(l) << N) {}
	constexpr Fix(int64_t l) noexcept : value(l << N) {}

	inline constexpr operator double() const noexcept { return static_cast<double>(value) / ONE; }
	inline constexpr operator float() const noexcept { return static_cast<float>(value) / ONE; }
	inline explicit constexpr operator int64_t() const noexcept {
		if(value >= 0 || (value & (ONE-1)) == 0) {
			return value >> N;
		} else {
			return (value >> N) + 1;
		}
	}

	inline Fix<N>& operator++() noexcept { value += ONE; return *this; }
	inline Fix<N> operator++(int) & noexcept  { Fix<N> old = *this; value += ONE; return old; }
	inline Fix<N>& operator--() noexcept { value -= ONE; return *this; }
	inline Fix<N> operator--(int) & noexcept  { Fix<N> old = *this; value -= ONE; return old; }

	inline Fix<N>& operator+=(const Fix<N>& b) noexcept { this->value += b.value; return *this; }
	inline Fix<N>& operator-=(const Fix<N>& b) noexcept { this->value -= b.value; return *this; }
	inline Fix<N>& operator+=(int64_t b) noexcept { this->value += (b << N); return *this; }
	inline Fix<N>& operator-=(int64_t b) noexcept { this->value -= (b << N); return *this; }
	inline Fix<N>& operator+=(double b) noexcept { this->value += static_cast<int64_t>(ONE * b); return *this; }
	inline Fix<N>& operator-=(double b) noexcept { this->value -= static_cast<int64_t>(ONE * b); return *this; }
	inline Fix<N>& operator+=(float b) noexcept { this->value += static_cast<int64_t>(ONE * static_cast<double>(b)); return *this; }
	inline Fix<N>& operator-=(float b) noexcept { this->value -= static_cast<int64_t>(ONE * static_cast<double>(b)); return *this; }

	inline constexpr Fix<N> operator-() const noexcept {Fix<N> result; result.value = -this->value; return result;}
};


template<int64_t N> inline constexpr Fix<N> operator+(Fix<N> a, Fix<N> b)   noexcept {Fix<N> result; result.value = a.value + b.value;  return result;}
template<int64_t N> inline constexpr Fix<N> operator-(Fix<N> a, Fix<N> b)   noexcept {Fix<N> result; result.value = a.value - b.value;  return result;}
template<int64_t N> inline constexpr Fix<N> operator+(Fix<N> a, int64_t b)  noexcept {Fix<N> result; result.value = a.value + (b << N); return result;}
template<int64_t N> inline constexpr Fix<N> operator-(Fix<N> a, int64_t b)  noexcept {Fix<N> result; result.value = a.value - (b << N); return result;}
template<int64_t N> inline constexpr Fix<N> operator+(int64_t a, Fix<N> b)  noexcept {Fix<N> result; result.value = (a << N) + b.value; return result;}
template<int64_t N> inline constexpr Fix<N> operator-(int64_t a, Fix<N> b)  noexcept {Fix<N> result; result.value = (a << N) - b.value; return result;}

template<int64_t N> inline constexpr Fix<N> operator+(Fix<N> a, double b)   noexcept {return a + Fix<N>(b);}
template<int64_t N> inline constexpr Fix<N> operator+(double a, Fix<N> b)   noexcept {return Fix<N>(a) + b;}
template<int64_t N> inline constexpr Fix<N> operator-(Fix<N> a, double b)   noexcept {return a - Fix<N>(b);}
template<int64_t N> inline constexpr Fix<N> operator-(double a, Fix<N> b)   noexcept {return Fix<N>(a) - b;}

template<int64_t N> inline constexpr Fix<N> operator+(Fix<N> a, float b)    noexcept {return a + Fix<N>(b);}
template<int64_t N> inline constexpr Fix<N> operator+(float a, Fix<N> b)    noexcept {return Fix<N>(a) + b;}
template<int64_t N> inline constexpr Fix<N> operator-(Fix<N> a, float b)    noexcept {return a - Fix<N>(b);}
template<int64_t N> inline constexpr Fix<N> operator-(float a, Fix<N> b)    noexcept {return Fix<N>(a) - b;}

template<int64_t N> inline constexpr Fix<N> operator*(Fix<N> a, int64_t b)  noexcept {Fix<N> result; result.value = a.value * b; return result;}
template<int64_t N> inline constexpr Fix<N> operator*(int64_t a, Fix<N> b)  noexcept {Fix<N> result; result.value = a * b.value; return result;}

template<int64_t N> inline constexpr Fix<N> operator*(Fix<N> a, int b)  noexcept { Fix<N> result; result.value = a.value * b; return result; }
template<int64_t N> inline constexpr Fix<N> operator*(int a, Fix<N> b)  noexcept { Fix<N> result; result.value = a * b.value; return result; }

template<int64_t N> inline constexpr Fix<N> operator*(Fix<N> a, double b)   noexcept {Fix<N> result; result.value = static_cast<int64_t>(a.value * b); return result;}
template<int64_t N> inline constexpr Fix<N> operator*(Fix<N> a, float b)    noexcept {Fix<N> result; result.value = static_cast<int64_t>(a.value * b); return result;}
template<int64_t N> inline constexpr Fix<N> operator*(double a, Fix<N> b)   noexcept {Fix<N> result; result.value = static_cast<int64_t>(a * b.value); return result;}
template<int64_t N> inline constexpr Fix<N> operator*(float a, Fix<N> b)    noexcept {Fix<N> result; result.value = static_cast<int64_t>(a * b.value); return result;}

template<int64_t N> inline constexpr Fix<N> operator/(Fix<N> a, double b)   noexcept {Fix<N> result; result.value = static_cast<int64_t>(a.value / b); return result;}
template<int64_t N> inline constexpr Fix<N> operator/(Fix<N> a, float b)    noexcept {Fix<N> result; result.value = static_cast<int64_t>(a.value / b); return result;}
template<int64_t N> inline constexpr Fix<N> operator/(double a, Fix<N> b)   noexcept {Fix<N> result; result.value = static_cast<int64_t>(a / b.value); return result;}
template<int64_t N> inline constexpr Fix<N> operator/(float a, Fix<N> b)    noexcept {Fix<N> result; result.value = static_cast<int64_t>(a / b.value); return result;}

template<int64_t N> inline constexpr Fix<N> operator/(Fix<N> a, int64_t b)  noexcept {Fix<N> result; result.value = a.value / b; return result;}
template<int64_t N> inline constexpr Fix<N> operator/(Fix<N> a, int b)  noexcept { Fix<N> result; result.value = a.value / b; return result; }

template<int64_t N> inline constexpr Fix<N> operator<<(Fix<N> a, int64_t b) noexcept {Fix<N> result; result.value = a.value << b; return result;}
template<int64_t N> inline constexpr Fix<N> operator>>(Fix<N> a, int64_t b) noexcept {Fix<N> result; result.value = a.value >> b; return result;}

template<int64_t N> inline constexpr Fix<N> operator<<(Fix<N> a, int b) noexcept { Fix<N> result; result.value = a.value << b; return result; }
template<int64_t N> inline constexpr Fix<N> operator>>(Fix<N> a, int b) noexcept { Fix<N> result; result.value = a.value >> b; return result; }


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

template<int64_t N>
inline constexpr Fix<N> min(Fix<N> first, Fix<N> second) noexcept {
	return (first.value <= second.value) ? first : second;
}

template<int64_t N>
inline constexpr Fix<N> max(Fix<N> first, Fix<N> second) noexcept {
	return (first.value >= second.value) ? first : second;
}
};