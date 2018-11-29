#pragma once

template<typename N>
struct Rot3Template {

	union {
		struct {
			N yaw;
			N pitch;
			N roll;
		};
		struct {
			N x;
			N y;
			N z;
		};
	};

	Rot3Template(N yaw, N pitch, N roll) : yaw(yaw), pitch(pitch), roll(roll) {};
	
	Rot3Template operator+(const Rot3Template& other) const {
		return Rot3Template(x + other.x, y + other.y, z + other.z);
	}

	Rot3Template operator-(const Rot3Template& other) const {
		return Rot3Template(x - other.x, y - other.y, z - other.z);
	}

	Rot3Template operator-() const {
		return Rot3Template(-x, -y, -z);
	}

	Rot3Template operator*(const Num& d) const {
		return Rot3Template(x*d, y*d, z*d);
	}

	Rot3Template operator/(const Num& d) const {
		return Rot3Template(x / d, y / d, z / d);
	}

	Num operator*(const Rot3Template& other) const {
		return this->x*other.x + this->y*other.y + this->z*other.z;
	}

	Rot3Template operator%(const Rot3Template& other) const {
		return Rot3Template(this->y*other.z - this->z*other.y, this->z*other.x - this->x*other.z, this->x*other.y - this->y*other.x);
	}
};

typedef Rot3Template<double>	Rot3;
typedef Rot3Template<float>		Rot3f;
typedef Rot3Template<long long>	Rot3f;

namespace Rot3Util {
	Rot3 ZERO = { 0, 0, 0 };
};
