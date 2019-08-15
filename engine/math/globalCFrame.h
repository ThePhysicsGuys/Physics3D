#pragma once

#include "position.h"
#include "mat3.h"
#include "mat4.h"
#include "cframe.h"

class GlobalCFrame {
public:
	Position position;
	Mat3 rotation;
	GlobalCFrame() : position(), rotation(1,0,0,0,1,0,0,0,1) {}
	GlobalCFrame(const Fix<32>& x, const Fix<32>& y, const Fix<32>& z) : position(x, y, z), rotation(1, 0, 0, 0, 1, 0, 0, 0, 1) {}
	GlobalCFrame(double x, double y, double z) : position(x, y, z), rotation(1, 0, 0, 0, 1, 0, 0, 0, 1) {}
	GlobalCFrame(const Position& position) : position(position), rotation(1,0,0,0,1,0,0,0,1) {}
	GlobalCFrame(const Mat3& rotation) : position(), rotation(rotation) {}
	GlobalCFrame(const Position& position, const Mat3& rotation) : position(position), rotation(rotation) {}

	inline Position getPosition() const {return position;}
	inline Mat3 getRotation() const { return rotation; }

	inline Vec3 globalToLocal(const Position& globalPos) const {
		return rotation.transpose() * Vec3(globalPos - this->position);
	}

	inline Position localToGlobal(const Vec3& localVec) const {
		return position + Vec3Fix(rotation * localVec);
	}

	inline Vec3 localToRelative(const Vec3& localVec) const {
		return rotation * localVec;
	}

	inline Vec3 relativeToLocal(const Vec3& relativeVec) const {
		return rotation.transpose() * relativeVec;
	}

	inline GlobalCFrame localToGlobal(const CFrame& localFrame) const {
		return GlobalCFrame(position + rotation * localFrame.getPosition(), rotation * localFrame.getRotation());
	}

	inline CFrame globalToLocal(const GlobalCFrame& globalFrame) const {
		return CFrame(rotation.transpose() * Vec3(globalFrame.position - position), rotation.transpose() * globalFrame.rotation);
	}

	inline Mat3 localToGlobal(const Mat3& localRot) const {
		return rotation * localRot;
	}

	inline Mat3 globalToLocal(const Mat3& globalRot) const {
		return rotation.transpose() * globalRot;
	}

	inline CFrame localToRelative(const CFrame& lFrame) const {
		return CFrame(rotation * lFrame.position, rotation * lFrame.rotation);
	}

	inline CFrame relativeToLocal(const CFrame& rFrame) const {
		return CFrame(rotation.transpose() * rFrame.position, rotation.transpose() * rFrame.rotation);
	}
	
	inline void translate(const Vec3& offset) {
		this->position += Vec3Fix(offset);
	}
	void rotate(Mat3 rot) {
		rotation = rot * rotation;
	}
};


inline Mat4 CFrameToMat4(const GlobalCFrame& cframe) {
	const Mat3& r = cframe.rotation;
	Vec3 p(double(cframe.position.x), double(cframe.position.y), double(cframe.position.z));
	return Mat4(
		r.m00, r.m01, r.m02, 0,
		r.m10, r.m11, r.m12, 0,
		r.m20, r.m21, r.m22, 0,
		p.x, p.y, p.z, 1
	);
}
