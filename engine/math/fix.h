#pragma once

template<__int64 N>
struct Fix;

#define ONE (1ULL << N)

template<__int64 N>
struct Fix {
	__int64 value;

	constexpr Fix() : value(0) {}
	Fix(double d) : 
		value() {
		
		__int64 uno = ONE;

		double newD = d * uno;

		this->value = static_cast<__int64>(newD);
	}
	constexpr Fix(float f) : 
		value(static_cast<__int64>(f * ONE)) {}
	constexpr explicit Fix(__int64 l) : value(l) {}

	inline constexpr operator double() const { return static_cast<double>(value) / ONE; }
	inline constexpr operator float() const { return static_cast<float>(value) / ONE; }

	inline Fix<N>& operator++() { value += 1 << N; return *this; }
	inline Fix<N>& operator++(int) { Fix<N> old = *this; value += ONE; return old; }
	inline Fix<N>& operator--() { value -= 1 << N; return *this; }
	inline Fix<N>& operator--(int) { Fix<N> old = *this; value -= ONE; return old; }

	inline Fix<N>& operator+=(const Fix<N>& b) { this->value += b.value; return *this; }
	inline Fix<N>& operator-=(const Fix<N>& b) { this->value -= b.value; return *this; }
	inline Fix<N>& operator+=(__int64 b) { this->value += (b << N); return *this; }
	inline Fix<N>& operator-=(__int64 b) { this->value -= (b << N); return *this; }

	static inline constexpr Fix<N> maxValue() {return Fix<N>(0x7FFFFFFF_FFFFFFFF); }
};

template<__int64 N> inline constexpr Fix<N> operator+(Fix<N> a, Fix<N> b) { return Fix<N>(a.value + b.value); }
template<__int64 N> inline constexpr Fix<N> operator-(Fix<N> a, Fix<N> b) { return Fix<N>(a.value - b.value); }
template<__int64 N> inline constexpr Fix<N> operator+(Fix<N> a, __int64 b) { return Fix<N>(a.value + (b << N)); }
template<__int64 N> inline constexpr Fix<N> operator-(Fix<N> a, __int64 b) { return Fix<N>(a.value - (b << N)); }
template<__int64 N> inline constexpr Fix<N> operator+(__int64 a, Fix<N> b) { return Fix<N>((a << N) + b.value); }
template<__int64 N> inline constexpr Fix<N> operator-(__int64 a, Fix<N> b) {return Fix<N>((a << N) - b.value);}

template<__int64 N> inline constexpr Fix<N> operator*(Fix<N> a, double b) {return Fix<N>(static_cast<__int64>(a.value * b));}
template<__int64 N> inline constexpr Fix<N> operator*(Fix<N> a, float b) {return Fix<N>(static_cast<__int64>(a.value * b));}
template<__int64 N> inline constexpr Fix<N> operator*(double a, Fix<N> b) {return Fix<N>(static_cast<__int64>(a * b.value));}
template<__int64 N> inline constexpr Fix<N> operator*(float a, Fix<N> b) {return Fix<N>(static_cast<__int64>(a * b.value));}

template<__int64 N> inline constexpr Fix<N> operator/(Fix<N> a, double b) { return Fix<N>(static_cast<__int64>(a.value / b)); }
template<__int64 N> inline constexpr Fix<N> operator/(Fix<N> a, float b) { return Fix<N>(static_cast<__int64>(a.value / b)); }
template<__int64 N> inline constexpr Fix<N> operator/(double a, Fix<N> b) { return Fix<N>(static_cast<__int64>(a / b.value)); }
template<__int64 N> inline constexpr Fix<N> operator/(float a, Fix<N> b) { return Fix<N>(static_cast<__int64>(a / b.value)); }

template<__int64 N> inline constexpr Fix<N> operator*(Fix<N> a, __int64 b) { return Fix<N>(a.value * b); }
template<__int64 N> inline constexpr Fix<N> operator*(__int64 a, Fix<N> b) { return Fix<N>(a * b.value); }

template<__int64 N> inline constexpr Fix<N> operator/(Fix<N> a, __int64 b) { return Fix<N>(static_cast<__int64>(a.value / b)); }

template<__int64 N> inline constexpr Fix<N> operator<<(Fix<N> a, __int64 b) { return Fix<N>(static_cast<__int64>(a.value << b)); }
template<__int64 N> inline constexpr Fix<N> operator>>(Fix<N> a, __int64 b) { return Fix<N>(static_cast<__int64>(a.value >> b)); }


#define CREATE_COMPARISONS(T1, T2, V1, V2) \
template<__int64 N> inline constexpr bool operator==(T1 a, T2 b) { return V1 == V2; } \
template<__int64 N> inline constexpr bool operator!=(T1 a, T2 b) { return V1 != V2; } \
template<__int64 N> inline constexpr bool operator>=(T1 a, T2 b) { return V1 >= V2; } \
template<__int64 N> inline constexpr bool operator<=(T1 a, T2 b) { return V1 <= V2; } \
template<__int64 N> inline constexpr bool operator>(T1 a, T2 b) { return V1 > V2; } \
template<__int64 N> inline constexpr bool operator<(T1 a, T2 b) { return V1 < V2; }

CREATE_COMPARISONS(Fix<N>, Fix<N>, a.value, b.value);
CREATE_COMPARISONS(Fix<N>, double, a, Fix<N>(b));
CREATE_COMPARISONS(double, Fix<N>, Fix<N>(a), b);
CREATE_COMPARISONS(Fix<N>, float, a, Fix<N>(b));
CREATE_COMPARISONS(float, Fix<N>, Fix<N>(a), b);
CREATE_COMPARISONS(Fix<N>, __int64, a.value, b << N);
CREATE_COMPARISONS(__int64, Fix<N>, a << N, b.value);
#undef CREATE_COMPARISONS

template<int N>
inline constexpr Fix<N> min(Fix<N> first, Fix<N> second) {
	return (first.value <= second.value)? first : second;
}

template<int N>
inline constexpr Fix<N> max(Fix<N> first, Fix<N> second) {
	return (first.value >= second.value) ? first : second;
}
#undef ONE

template<__int64 N>
Fix<N> quickMultiply(Fix<N> a, Fix<N> b) {
	return Fix<N>((a.value >> (N / 2)) * (b.value >> (N - N / 2)));
}
