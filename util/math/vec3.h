#pragma once

#include <math.h>

template<typename Num>
struct Vec3Template {
public:
	Num x;
	Num y;
	Num z;

	
	Vec3Template() : x(0), y(0), z(0) {}
	Vec3Template(Num x, Num y, Num z) : x(x), y(y), z(z) {};
	Vec3Template(const Vec3Template& other) : x(other.x), y(other.y), z(other.z) {};
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

	Vec3Template operator*(const Num& d) const {
		return Vec3Template(x*d, y*d, z*d);
	}

	Vec3Template operator/(const Num& d) const {
		return Vec3Template(x / d, y / d, z / d);
	}

	Num operator*(const Vec3Template& other) const {
		return this->x*other.x + this->y*other.y + this->z*other.z;
	}

	Vec3Template operator%(const Vec3Template& other) const {
		return Vec3Template(this->y*other.z - this->z*other.y, this->z*other.x - this->x*other.z, this->x*other.y - this->y*other.x);
	}

	Num length() const {
		return sqrt(lengthSquared());
	}

	Num lengthSquared() const {
		return x*x + y*y + z*z;
	}

	Vec3Template withLength(Num newLength) const {
		Num f = newLength / this->length();
		return Vec3Template(x*f, y*f, z*f);
	}

	Vec3Template maxLength(Num maxLength) const {
		Num lengthSquared = this->lengthSquared();
		if (maxLength*maxLength < lengthSquared) {
			Num f = maxLength / length();
			return Vec3Template(x*f, y*f, z*f);
		}
		else
			return *this;
	}

	Vec3Template minLength(Num maxLength) const {
		Num lengthSquared = this->lengthSquared();
		if (maxLength*maxLength > lengthSquared) {
			Num f = maxLength / length();
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
	Vec3Template reProject(Num v) const {
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

	double angleBetween(const Vec3Template other) const {
		return acos(this->normalize() * (other.normalize()));
	}

	bool isLongerThan(Num length) const {
		return lengthSquared() > length*length;
	}

	bool isShorterThan(Num length) const {
		return lengthSquared() < length*length;
	}

	/**
	* returns the distance of the given point to the line that goes through the origin along this vector
	* @param point
	* @return the distance
	*/
	Num pointToLineDistance(const Vec3Template point) const {
		return (point % (*this)).length() / length();
	}

	/**
	* returns the squared distance of the given point to the line that goes through the origin along this vector
	* @param point
	* @return the square of the distance
	*/
	Num pointToLineDistanceSquared(const Vec3Template point) const {
		Num crossProd = (point % *this).length();
		return crossProd*crossProd / lengthSquared();
	}

	Vec3Template bisect(const Vec3Template other) const {
		return *this * other.length() + other * (*this).length();
	}
};

typedef Vec3Template<double>	Vec3;
typedef Vec3Template<float>		Vec3F;
typedef Vec3Template<long long>	Vec3L;



namespace Vec3Util{
	const Vec3 ZERO(0.0, 0.0, 0.0);
	const Vec3 UNITX(1.0, 0.0, 0.0);
	const Vec3 UNITY(0.0, 1.0, 0.0);
	const Vec3 UNITZ(0.0, 0.0, 1.0);
	const Vec3 UNITNEGX(-1.0, 0.0, 0.0);
	const Vec3 UNITNEGY(0.0, -1.0, 0.0);
	const Vec3 UNITNEGZ(0.0, 0.0, -1.0);
};
