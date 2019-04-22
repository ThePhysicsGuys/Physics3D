#pragma once

struct CFrame;

#include "vec3.h"
#include "mat3.h"
#include "mat4.h"

struct CFrame {
public:
	Vec3 position;
	Mat3 rotation;

	CFrame(Vec3 position, Mat3 rotation) : position(position), rotation(rotation) {}
	CFrame(Vec3 position) : position(position), rotation(Mat3(1, 0, 0, 0, 1, 0, 0, 0, 1)) {}
	CFrame(Mat3 rotation) : position(Vec3(0,0,0)), rotation(rotation) {}
	CFrame() : position(Vec3(0, 0, 0)), rotation(Mat3(1, 0, 0, 0, 1, 0, 0, 0, 1)) {}

	Mat4 asMat4() const;
	Mat4f asMat4f() const;

	inline Vec3 localToGlobal(Vec3 lVec) const {
		return rotation * lVec + position;
	}

	inline Vec3 globalToLocal(Vec3 gVec) const {
		return rotation.transpose() * (gVec - position);
	}

	inline Vec3 localToRelative(Vec3 lVec) const {
		return rotation * lVec;
	}

	inline Vec3 relativeToLocal(Vec3 rVec) const {
		return rotation.transpose() * rVec;
	}

	inline CFrame localToGlobal(CFrame lFrame) const {
		return CFrame(position + rotation * lFrame.position, rotation * lFrame.rotation);
	}

	inline CFrame globalToLocal(CFrame gFrame) const {
		return CFrame(rotation.transpose()*(gFrame.position - position) , rotation.transpose() * gFrame.rotation);
	}
	
	inline CFrame localToRelative(CFrame lFrame) const {
		return CFrame(rotation * lFrame.position, rotation * lFrame.rotation);
	}

	inline CFrame relativeToLocal(CFrame rFrame) const {
		return CFrame(rotation.transpose()*rFrame.position, rotation.transpose() * rFrame.rotation);
	}

	inline CFrame operator~() const {
		return CFrame(rotation.transpose() * -position, rotation.transpose());
	}

	CFrame& operator+=(const Vec3& delta);
	CFrame& operator-=(const Vec3& delta);

	void translate(Vec3 translation);
	void rotate(RotMat3 rotation);
};

CFrame operator+(const CFrame& frame, const Vec3& delta);
CFrame operator+(const Vec3& delta, const CFrame& frame);
CFrame operator-(const CFrame& frame, const Vec3& delta);
CFrame operator-(const Vec3& delta, const CFrame& frame);
