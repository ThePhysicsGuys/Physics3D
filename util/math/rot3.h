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
};

typedef Rot3Template<double>	Rot3;
typedef Rot3Template<float>		Rot3f;
typedef Rot3Template<long long>	Rot3f;

namespace Rot3Util {
	Rot3 ZERO = { 0, 0, 0 };
};
