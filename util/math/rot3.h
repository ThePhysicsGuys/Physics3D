#pragma once

struct Rot3 {

	union {
		struct {
			double yaw;
			double pitch;
			double roll;
		};
		struct {
			double x;
			double y;
			double z;
		};
	};

	Rot3(double yaw, double pitch, double roll) : yaw(yaw), pitch(pitch), roll(roll) {};
};

namespace Rot3Util {
	Rot3 ZERO = { 0, 0, 0 };
};
