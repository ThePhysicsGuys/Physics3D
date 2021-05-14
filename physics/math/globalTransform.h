#pragma once



#include "position.h"
#include "linalg/mat.h"
#include "transform.h"
#include "globalCFrame.h"

namespace P3D {
class GlobalTransform {
public:
	Position position;
	Mat3 transform;
	GlobalTransform() : position(), transform(Mat3::IDENTITY()) {}
	GlobalTransform(const Fix<32>& x, const Fix<32>& y, const Fix<32>& z) : position(x, y, z), transform(Mat3::IDENTITY()) {}
	GlobalTransform(double x, double y, double z) : position(x, y, z), transform(Mat3::IDENTITY()) {}
	GlobalTransform(double x, double y, double z, const Mat3& transform) : position(x, y, z), transform(transform) {}
	GlobalTransform(const Position& position) : position(position), transform(Mat3::IDENTITY()) {}
	GlobalTransform(const Mat3& transform) : position(), transform(transform) {}
	GlobalTransform(const Position& position, const Mat3& transform) : position(position), transform(transform) {}

	GlobalTransform(const GlobalCFrame& cframe) : position(cframe.getPosition()), transform(cframe.getRotation()) {}

	inline Position getPosition() const { return position; }
	inline Mat3 getTransform() const { return transform; }

	inline Vec3 globalToLocal(const Position& globalPos) const {
		return ~transform * Vec3(globalPos - this->position);
	}

	inline Position localToGlobal(const Vec3& localVec) const {
		return position + Vec3Fix(transform * localVec);
	}

	inline Vec3 localToRelative(const Vec3& localVec) const {
		return transform * localVec;
	}

	inline Vec3 relativeToLocal(const Vec3& relativeVec) const {
		return ~transform * relativeVec;
	}

	inline GlobalTransform localToGlobal(const Transform& localTransform) const {
		return GlobalTransform(position + transform * localTransform.getPosition(), transform * localTransform.getLocalTransformation());
	}

	inline Transform globalToLocal(const GlobalTransform& globalFrame) const {
		return Transform(~transform * Vec3(globalFrame.position - position), ~transform * globalFrame.transform);
	}

	inline Mat3 localToGlobal(const Mat3& localRot) const {
		return transform * localRot;
	}

	inline Mat3 globalToLocal(const Mat3& globalRot) const {
		return ~transform * globalRot;
	}

	inline Transform localToRelative(const Transform& lTransform) const {
		return Transform(transform * lTransform.position, transform * lTransform.transform);
	}

	inline Transform relativeToLocal(const Transform& lTransform) const {
		return Transform(~transform * lTransform.position, ~transform * lTransform.transform);
	}

	inline void translate(const Vec3Fix& offset) {
		this->position += offset;
	}
	inline GlobalTransform translated(const Vec3Fix& offset) const {
		return GlobalTransform(position + offset);
	}
	inline void rotate(const Mat3& rot) {
		this->transform = rot * this->transform;
	}

	inline GlobalTransform rotated(const Mat3& rot) const {
		return GlobalTransform(position, rot * transform);
	}
};
};
