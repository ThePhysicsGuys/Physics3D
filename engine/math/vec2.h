#pragma once

template<typename N>
struct Vec2Template {
public:

	N x;
	N y;

	Vec2Template() : x(0), y(0) {}
	Vec2Template(N x, N y) : x(x), y(y) {};
	Vec2Template(const Vec2Template& other) : x(other.x), y(other.y) {};

	Vec2Template operator+(const Vec2Template& other) const {
		return Vec2Template(x + other.x, y + other.y);
	}

	Vec2Template operator-(const Vec2Template& other) const {
		return Vec2Template(x - other.x, y - other.y);
	}

	Vec2Template operator-() const {
		return Vec2Template(-x, -y);
	}

	Vec2Template& operator+=(const Vec2Template& other) {
		this->x += other.x;
		this->y += other.y;
		return *this;
	}

	Vec2Template& operator-=(const Vec2Template& other) {
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}

	Vec2Template& operator*=(const N& factor) {
		this->x *= factor;
		this->y *= factor;
		return *this;
	}

	Vec2Template& operator/=(const N& factor) {
		this->x /= factor;
		this->y /= factor;
		return *this;
	}

	Vec2Template operator*(const N& d) const {
		return Vec2Template(x*d, y*d);
	}
	
	Vec2Template operator/(const N& d) const {
		return Vec2Template(x / d, y / d);
	}

	N operator*(const Vec2Template& other) const {
		return this->x*other.x + this->y*other.y;
	}

	N operator%(const Vec2Template& other) const {
		return this->x*other.y - this->y*other.x;
	}

	N length() const;

	N lengthSquared() const {
		return x * x + y * y;
	}
};

typedef Vec2Template<double>	Vec2;
typedef Vec2Template<float>		Vec2f;
typedef Vec2Template<long long>	Vec2l;
