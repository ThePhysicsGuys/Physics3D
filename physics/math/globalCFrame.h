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

	inline Vec3 globalToRelative(const Position& gPos) const {
		return gPos - position;
	}

	inline Position relativeToGlobal(const Vec3& rVec) const {
		return position + rVec;
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

	inline void rotate(const Rotation& rot) {
		this->rotation = rot * this->rotation;
	}
	inline GlobalCFrame rotated(const Rotation& rot) const {
		return GlobalCFrame(position, rot * rotation);
	}
	
	inline GlobalCFrame& operator+=(const Vec3Fix& offset) {
		this->position += offset;
		return *this;
	}
	inline GlobalCFrame& operator-=(const Vec3Fix& offset) {
		this->position -= offset;
		return *this;
	}
	inline GlobalCFrame operator+(const Vec3Fix& offset) const {
		return GlobalCFrame(position + offset, this->rotation);
	}
	inline GlobalCFrame operator-(const Vec3Fix& offset) const {
		return GlobalCFrame(position - offset, this->rotation);
	}

	inline GlobalCFrame extendLocal(const Vec3& offset) const {
		return GlobalCFrame(this->localToGlobal(offset), this->rotation);
	}

	/*
		Converts this CFrame to a 4x4 matrix, where for any Vec3 p:
		cframe.asMat4() * Vec4(p, 1.0) == cframe.localToGlobal(p)

		Note: loss of precision for far out positions, it is recommended to first compute the relative position, and then construct this matrix. 
		This is because the position is unsafely cast to a Vec3. 
	*/
	Mat4 asMat4() const {
		return Mat4(rotation.asRotationMatrix(), castPositionToVec3(this->position), Vec3(0.0, 0.0, 0.0), 1.0);
	}

	/*
		Converts this CFrame to a 4x4 matrix with given scaling factor, where for any Vec3 p:
		cframe.asMat4WithPreScale(scale) * Vec4(p, 1.0) == cframe.localToGlobal(scale * p)

		Note: loss of precision for far out positions, it is recommended to first compute the relative position, and then construct this matrix. 
		This is because the position is unsafely cast to a Vec3. 
	*/
	Mat4 asMat4WithPreScale(const DiagonalMat3& scale) const {
		return Mat4(rotation.asRotationMatrix() * scale, castPositionToVec3(this->position), Vec3(0.0, 0.0, 0.0), 1.0);
	}

	/*
		Converts this CFrame to a 4x4 matrix with given scaling factor, where for any Vec3 p:
		cframe.asMat4WithPostScale(scale) * Vec4(p, 1.0) == scale * cframe.localToGlobal(p)

		Note: loss of precision for far out positions, it is recommended to first compute the relative position, and then construct this matrix. 
		This is because the position is unsafely cast to a Vec3. 
	*/
	Mat4 asMat4WithPostScale(const DiagonalMat3& scale) const {
		return Mat4(scale * rotation.asRotationMatrix(), castPositionToVec3(this->position), Vec3(0.0, 0.0, 0.0), 1.0);
	}
};


inline CFrame operator-(GlobalCFrame cf, Position relativeTo) {
	return CFrame(cf.getPosition() - relativeTo, cf.getRotation());
}
inline GlobalCFrame operator+(Position relativeTo, CFrame cf) {
	return GlobalCFrame(relativeTo + cf.getPosition(), cf.getRotation());
}
