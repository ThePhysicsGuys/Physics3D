#pragma once

template<typename N>
struct Vec4Template {
public:

	N x;
	N y;
	N z;
	N w;

	Vec4Template() : x(0), y(0), z(0), w(0) {};
	Vec4Template(N v) : x(v), y(v), z(v), w(v) {};
	Vec4Template(N x, N y, N z, N w) : x(x), y(y), z(z), w(w) {};
	Vec4Template(const Vec4Template<float>& other) : x(other.x), y(other.y), z(other.z), w(other.w) {};
	Vec4Template(const Vec4Template<double>& other) : x(other.x), y(other.y), z(other.z), w(other.w) {};
	Vec4Template(const Vec4Template<long long>& other) : x(other.x), y(other.y), z(other.z), w(other.w) {};

	Vec4Template operator+(const Vec4Template& other) const {
		return Vec4Template(x + other.x, y + other.y, z + other.z, w + other.w);
	}

	Vec4Template operator-(const Vec4Template& other) const {
		return Vec4Template(x - other.x, y - other.y, z - other.z, w - other.w);
	}

	Vec4Template operator-() const {
		return Vec4Template(-x, -y, -z, -w);
	}

	Vec4Template& operator+=(const Vec4Template& other) {
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		this->w += other.w;
		return *this;
	}

	Vec4Template& operator-=(const Vec4Template& other) {
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		this->w -= other.w;
		return *this;
	}

	Vec4Template& operator*=(const N& factor) {
		this->x *= factor;
		this->y *= factor;
		this->z *= factor;
		this->w *= factor;
		return *this;
	}

	Vec4Template& operator/=(const N& factor) {
		this->x /= factor;
		this->y /= factor;
		this->z /= factor;
		this->w /= factor;
		return *this;
	}

	Vec4Template operator*(const N& d) const {
		return Vec4Template(x * d, y * d, z * d, w * d);
	}

	Vec4Template operator/(const N& d) const {
		return Vec4Template(x / d, y / d, z / d, w / d);
	}

	N operator*(const Vec4Template& other) const {
		return this->x * other.x + this->y * other.y + this->z * other.z + this->w * other.w;
	}

	N length() const;

	N lengthSquared() const {
		return x * x + y * y + z * z + w * w;
	}
};

typedef Vec4Template<double>	Vec4;
typedef Vec4Template<float>		Vec4f;
typedef Vec4Template<long long>	Vec4l;
