#pragma once

template<typename N>
struct Vec3Template {
public:
	union {
		struct {
			N x;
			N y;
			N z;
		};
		N v[3];
	};

	Vec3Template() : x(0), y(0), z(0) {}
	explicit Vec3Template(N v) : x(v), y(v), z(v) {};
	Vec3Template(N x, N y, N z) : x(x), y(y), z(z) {};
	template<typename OtherN>
	Vec3Template(const Vec3Template<OtherN>& other) : x(static_cast<N>(other.x)), y(static_cast<N>(other.y)), z(static_cast<N>(other.z)) {};
	~Vec3Template() {};
	
	Vec3Template operator+(const Vec3Template& other) const {
		return Vec3Template(x + other.x, y + other.y, z + other.z);
	}

	Vec3Template operator-(const Vec3Template& other) const {
		return Vec3Template(x - other.x, y - other.y, z - other.z);
	}

	Vec3Template operator-() const {
		return Vec3Template(-x, -y, -z);
	}

	Vec3Template& operator+=(const Vec3Template& other) {
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}

	Vec3Template& operator-=(const Vec3Template& other) {
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		return *this;
	}

	Vec3Template& operator*=(const N& factor) {
		this->x *= factor;
		this->y *= factor;
		this->z *= factor;
		return *this;
	}

	Vec3Template& operator/=(const N& factor) {
		this->x /= factor;
		this->y /= factor;
		this->z /= factor;
		return *this;
	}

	Vec3Template operator*(const N& d) const {
		return Vec3Template(x*d, y*d, z*d);
	}

	Vec3Template operator/(const N& d) const {
		return Vec3Template(x / d, y / d, z / d);
	}

	N operator*(const Vec3Template& other) const {
		return this->x*other.x + this->y*other.y + this->z*other.z;
	}

	Vec3Template operator%(const Vec3Template& other) const {
		return Vec3Template(this->y*other.z - this->z*other.y, 
							this->z*other.x - this->x*other.z, 
							this->x*other.y - this->y*other.x);
	}

	N& operator[](int index) {
		return v[index];
	}

	Vec3Template abs() const;

	N length() const;

	N lengthSquared() const {
		return x*x + y*y + z*z;
	}

	Vec3Template withLength(N newLength) const {
		N f = newLength / this->length();
		return Vec3Template(x*f, y*f, z*f);
	}

	Vec3Template maxLength(N maxLength) const {
		N lengthSquared = this->lengthSquared();
		if (maxLength*maxLength < lengthSquared) {
			N f = maxLength / length();
			return Vec3Template(x*f, y*f, z*f);
		}
		else
			return *this;
	}

	Vec3Template minLength(N minLength) const {
		N lengthSquared = this->lengthSquared();
		if (minLength*minLength > lengthSquared) {
			N f = minLength / length();
			return Vec3Template(x*f, y*f, z*f);
		}
		else
			return *this;
	}

	Vec3Template normalize() const {
		return *this / this->length();
	}

	/**
	* used to project the result of a dotproduct back onto the original vector
	* @param v the result of this->dot(someVec)
	* @return this vector times (v/lengthSquared())
	*/
	Vec3Template reProject(N v) const {
		return *this * (v / lengthSquared());
	}

	/**
	* projects other on this
	* @param other vector to be projected
	* @return a projected version of the given vector
	*/
	Vec3Template project(const Vec3Template other) const {
		return *this * ((*this * other) / lengthSquared());
	}

	N angleBetween(const Vec3Template other) const;

	Vec3Template rotateAround(const Vec3Template other, N angle) const;

	bool isLongerThan(N length) const {
		return lengthSquared() > length*length;
	}

	bool isShorterThan(N length) const {
		return lengthSquared() < length*length;
	}

	/**
	* returns the distance of the given point to the line that goes through the origin along this vector
	* @param point
	* @return the distance
	*/
	N pointToLineDistance(const Vec3Template point) const {
		return (point % (*this)).length() / length();
	}

	/**
	* returns the squared distance of the given point to the line that goes through the origin along this vector
	* @param point
	* @return the square of the distance
	*/
	N pointToLineDistanceSquared(const Vec3Template point) const {
		N crossProd = (point % *this).length();
		return crossProd*crossProd / lengthSquared();
	}

	Vec3Template bisect(const Vec3Template other) const {
		return *this * other.length() + other * (*this).length();
	}

	Vec3Template mul(Vec3Template other) const {
		return Vec3Template(x*other.x, y*other.y, z*other.z);
	}

	Vec3Template squared() const {
		return Vec3Template(x*x, y*y, z*z);
	}

	Vec3Template cubed() const {
		return Vec3Template(x*x*x, y*y*y, z*z*z);
	}
};

template<typename N>
Vec3Template<N> operator*(const double& factor, const Vec3Template<N>& vec) { return vec * factor; }
template<typename N>
Vec3Template<N> operator*(const float& factor, const Vec3Template<N>& vec) { return vec * factor; }

typedef Vec3Template<double>	Vec3;
typedef Vec3Template<float>		Vec3f;
typedef Vec3Template<long long>	Vec3l;
