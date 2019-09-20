#pragma once

#include "position.h"
#include "linalg/mat.h"
#include "cframe.h"

class GlobalCFrame {
public:
	Position position;
	Mat3 rotation;
	GlobalCFrame() : position(), rotation(Mat3::IDENTITY()) {}
	GlobalCFrame(const Fix<32>& x, const Fix<32>& y, const Fix<32>& z) : position(x, y, z), rotation(Mat3::IDENTITY()) {}
	GlobalCFrame(double x, double y, double z) : position(x, y, z), rotation(Mat3::IDENTITY()) {}
	GlobalCFrame(const Position& position) : position(position), rotation(Mat3::IDENTITY()) {}
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
	
	inline void translate(const Vec3Fix& offset) {
		this->position += offset;
	}
	inline GlobalCFrame translated(const Vec3Fix& offset) const {
		return GlobalCFrame(position + offset);
	}
	inline void rotate(const Mat3& rot) {
		this->rotation = rot * this->rotation;
	}

	inline GlobalCFrame rotated(const Mat3& rot) const {
		return GlobalCFrame(position, rot * rotation);
	}
};

inline Mat4 TransformToMat4(const Position& position, const Mat3& r) {
	Vec3 p(double(position.x), double(position.y), double(position.z));
	return Mat4(Matrix<double, 3, 3>(r), p, Vec3(0, 0, 0), 1.0);
}
inline Mat4 CFrameToMat4(const GlobalCFrame& cframe) {
	return TransformToMat4(cframe.getPosition(), cframe.getRotation());
}



