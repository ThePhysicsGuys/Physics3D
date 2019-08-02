#pragma once

#include "position.h"
#include "mat3.h"
#include "cframe.h"

class GlobalCFrame {
public:
	Position position;
	Mat3 rotation;
	GlobalCFrame() : position(), rotation(1,0,0,0,1,0,0,0,1) {}
	GlobalCFrame(const Position& position) : position(position), rotation(1,0,0,0,1,0,0,0,1) {}
	GlobalCFrame(const Mat3& rotation) : position(), rotation(rotation) {}
	GlobalCFrame(const Position& position, const Mat3& rotation) : position(position), rotation(rotation) {}

	inline Position getPosition() {return position;}
	inline Mat3 getRotation() { return rotation; }

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

	inline GlobalCFrame localToGlobal(const CFrame& localFrame) {
		return GlobalCFrame(position + rotation * localFrame.getPosition(), rotation * localFrame.getRotation());
	}

	inline CFrame globalToLocal(const GlobalCFrame& globalFrame) const {
		return CFrame(rotation.transpose() * Vec3(globalFrame.position - position), rotation.transpose() * globalFrame.rotation);
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
};

