#pragma once

struct CFrame;

#include "vec3.h"
#include "mat3.h"
#include "mat4.h"

struct CFrame {
public:
	Vec3 position;
	Mat3 rotation;

	CFrame(Vec3 position, Mat3 rotation) : position(position), rotation(rotation) {};

	Mat4 asMat4() const;

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