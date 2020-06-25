#pragma once

#include "linalg/vec.h"
#include "rotation.h"

template<typename T>
struct CFrameTemplate {
public:
	Vector<T, 3> position;
	RotationTemplate<T> rotation;

	CFrameTemplate(const Vector<T, 3>& position, const RotationTemplate<T>& rotation) : position(position), rotation(rotation) {}
	explicit CFrameTemplate(const Vector<T, 3>& position) : position(position), rotation() {}
	explicit CFrameTemplate(const T& x, const T& y, const T& z) : position(x, y, z), rotation() {}
	explicit CFrameTemplate(const T& x, const T& y, const T& z, const RotationTemplate<T>& rotation) : position(x, y, z), rotation(rotation) {}
	explicit CFrameTemplate(const RotationTemplate<T>& rotation) : position(0,0,0), rotation(rotation) {}
	CFrameTemplate() : position(0, 0, 0), rotation() {}

	template<typename OtherT>
	CFrameTemplate(const CFrameTemplate<OtherT>& other) : 
		position(static_cast<Vector<T, 3>>(other.position)),
		rotation(static_cast<RotationTemplate<T>>(other.rotation)) {}

	Vector<T, 3> localToGlobal(const Vector<T, 3>& lVec) const {
		return rotation.localToGlobal(lVec) + position;
	}

	Vector<T, 3> globalToLocal(const Vector<T, 3>& gVec) const {
		return rotation.globalToLocal(gVec - position);
	}

	Vector<T, 3> localToRelative(const Vector<T, 3>& lVec) const {
		return rotation.localToGlobal(lVec);
	}

	Vector<T, 3> relativeToLocal(const Vector<T, 3>& rVec) const {
		return rotation.globalToLocal(rVec);
	}

	CFrameTemplate<T> localToGlobal(const CFrameTemplate<T>& lFrame) const {
		return CFrameTemplate<T>(position + rotation.localToGlobal(lFrame.position), rotation.localToGlobal(lFrame.rotation));
	}

	CFrameTemplate<T> globalToLocal(const CFrameTemplate<T>& gFrame) const {
		return CFrameTemplate<T>(rotation.globalToLocal(gFrame.position - position), rotation.globalToLocal(gFrame.rotation));
	}
	
	CFrameTemplate<T> localToRelative(const CFrameTemplate<T>& lFrame) const {
		return CFrameTemplate<T>(rotation.localToGlobal(lFrame.position), rotation.localToGlobal(lFrame.rotation));
	}

	CFrameTemplate<T> relativeToLocal(const CFrameTemplate<T>& rFrame) const {
		return CFrameTemplate<T>(rotation.globalToLocal(rFrame.position), rotation.globalToLocal(rFrame.rotation));
	}

	RotationTemplate<T> localToGlobal(const RotationTemplate<T>& localRot) const {
		return rotation.localToGlobal(localRot);
	}

	RotationTemplate<T> globalToLocal(const RotationTemplate<T>& globalRot) const {
		return rotation.globalToLocal(globalRot);
	}

	CFrameTemplate<T> operator~() const {
		return CFrameTemplate<T>(rotation.globalToLocal(-position), ~rotation);
	}

	Vector<T, 3> getPosition() const {
		return position;
	}

	RotationTemplate<T> getRotation() const {
		return rotation;
	}

	CFrameTemplate<T>& operator+=(const Vector<T, 3>& delta) {
		position += delta;
		return *this;
	}

	CFrameTemplate<T>& operator-=(const Vector<T, 3>& delta) {
		position -= delta;
		return *this;
	}

	CFrameTemplate<T> operator+(const Vector<T, 3>& delta) const {
		return CFrameTemplate<T>(this->position + delta, this->rotation);
	}

	CFrameTemplate<T> operator-(const Vector<T, 3>& delta) const {
		return CFrameTemplate<T>(this->position - delta, this->rotation);
	}

	/*
		Converts this CFrame to a 4x4 matrix, where for any Vec3 p:
		cframe.asMat4() * Vec4(p, 1.0) == cframe.localToGlobal(p)
	*/
	Matrix<T, 4, 4> asMat4() const {
		return Matrix<T, 4, 4>(rotation.asRotationMatrix(), this->position, Vector<T, 3>::ZEROS(), static_cast<T>(1));
	}

	/*
		Converts this CFrame to a 4x4 matrix with given scaling factor, where for any Vec3 p:
		cframe.asMat4WithPreScale(scale) * Vec4(p, 1.0) == cframe.localToGlobal(scale * p)
	*/
	Matrix<T, 4, 4> asMat4WithPreScale(const DiagonalMatrix<T, 3>& scale) const {
		return Matrix<T, 4, 4>(rotation.asRotationMatrix() * scale, this->position, Vector<T, 3>::ZEROS(), static_cast<T>(1));
	}

	/*
		Converts this CFrame to a 4x4 matrix with given scaling factor, where for any Vec3 p:
		cframe.asMat4WithPostScale(scale) * Vec4(p, 1.0) == scale * cframe.localToGlobal(p)
	*/
	Matrix<T, 4, 4> asMat4WithPostScale(const DiagonalMatrix<T, 3>& scale) const {
		return Matrix<T, 4, 4>(scale * rotation.asRotationMatrix(), this->position, Vector<T, 3>::ZEROS(), static_cast<T>(1));
	}
};

typedef CFrameTemplate<double> CFrame;
typedef CFrameTemplate<float> CFramef;
