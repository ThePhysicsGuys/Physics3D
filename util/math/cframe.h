#pragma once

#include "vec3.h"

struct RotMat3 {
	Vec3 operator*(const Vec3& v) const { return Vec3(); };
	RotMat3 operator*(const RotMat3& m) const { return RotMat3(); }
	RotMat3 operator~() const { return RotMat3(); };
};

struct CFrame {
public:
	Vec3 position;
	RotMat3 rotation;

	CFrame(Vec3 position, RotMat3 rotation) : position(position), rotation(rotation) {};

	Vec3 localToGlobal(Vec3 lVec) const {
		return rotation * lVec + position;
	}

	Vec3 globalToLocal(Vec3 gVec) const {
		return ~rotation * (gVec - position);
	}

	CFrame localToGlobal(CFrame lFrame) const {
		return CFrame(position + rotation * lFrame.position, rotation * lFrame.rotation);
	}

	CFrame globalToLocal(CFrame gFrame) const {
		return CFrame(~rotation*(gFrame.position - position) , ~rotation * gFrame.rotation);
	}
};
