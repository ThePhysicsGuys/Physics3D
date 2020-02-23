#pragma once

#include "position.h"
#include "rotation.h"
#include "cframe.h"

class GlobalCFrame {
public:
	Position position;
	Rotation rotation;
	GlobalCFrame() : position(), rotation() {}
	GlobalCFrame(const Fix<32>& x, const Fix<32>& y, const Fix<32>& z) : position(x, y, z), rotation() {}
	GlobalCFrame(double x, double y, double z) : position(x, y, z), rotation() {}
	GlobalCFrame(double x, double y, double z, const Rotation& rotation) : position(x, y, z), rotation(rotation) {}
	GlobalCFrame(const Position& position) : position(position), rotation() {}
	GlobalCFrame(const Rotation& rotation) : position(), rotation(rotation) {}
	GlobalCFrame(const Position& position, const Rotation& rotation) : position(position), rotation(rotation) {}

	inline Position getPosition() const {return position;}
	inline Rotation getRotation() const { return rotation; }

	inline Vec3 globalToLocal(const Position& globalPos) const {
		return rotation.globalToLocal(Vec3(globalPos - this->position));
	}

	inline Position localToGlobal(const Vec3& localVec) const {
		return position + Vec3Fix(rotation.localToGlobal(localVec));
	}

	inline Vec3 localToRelative(const Vec3& localVec) const {
		return rotation.localToGlobal(localVec);
	}

	inline Vec3 relativeToLocal(const Vec3& relativeVec) const {
		return rotation.globalToLocal(relativeVec);
	}

	inline GlobalCFrame localToGlobal(const CFrame& localFrame) const {
		return GlobalCFrame(position + rotation.localToGlobal(localFrame.getPosition()), rotation * localFrame.getRotation());
	}

	inline CFrame globalToLocal(const GlobalCFrame& globalFrame) const {
		return CFrame(rotation.globalToLocal(Vec3(globalFrame.position - position)), ~rotation * globalFrame.rotation);
	}

	inline Rotation localToGlobal(const Rotation& localRot) const {
		return rotation * localRot;
	}

	inline Rotation globalToLocal(const Rotation& globalRot) const {
		return ~rotation * globalRot;
	}

	inline CFrame localToRelative(const CFrame& lFrame) const {
		return CFrame(rotation * lFrame.position, rotation * lFrame.rotation);
	}

	inline CFrame relativeToLocal(const CFrame& rFrame) const {
		return CFrame(rotation.globalToLocal(rFrame.position), ~rotation * rFrame.rotation);
	}
	
	inline void translate(const Vec3Fix& offset) {
		this->position += offset;
	}
	inline GlobalCFrame translated(const Vec3Fix& offset) const {
		return GlobalCFrame(position + offset);
	}
	inline void rotate(const Rotation& rot) {
		this->rotation = rot * this->rotation;
	}

	inline GlobalCFrame rotated(const Rotation& rot) const {
		return GlobalCFrame(position, rot * rotation);
	}
};
