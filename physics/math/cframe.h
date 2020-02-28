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
	explicit CFrameTemplate(const RotationTemplate<T>& rotation) : position(0,0,0), rotation(rotation) {}
	CFrameTemplate() : position(0, 0, 0), rotation() {}

	template<typename OtherT>
	CFrameTemplate(const CFrameTemplate<OtherT>& other) : 
		position(Vector<T, 3>(other.position)),
		rotation(RotationTemplate<T>(other.rotation)) {}

	inline Vector<T, 3> localToGlobal(const Vector<T, 3>& lVec) const {
		return rotation.localToGlobal(lVec) + position;
	}

	inline Vector<T, 3> globalToLocal(const Vector<T, 3>& gVec) const {
		return rotation.globalToLocal(gVec - position);
	}

	inline Vector<T, 3> localToRelative(const Vector<T, 3>& lVec) const {
		return rotation.localToGlobal(lVec);
	}

	inline Vector<T, 3> relativeToLocal(const Vector<T, 3>& rVec) const {
		return rotation.globalToLocal(rVec);
	}

	inline CFrameTemplate<T> localToGlobal(const CFrameTemplate<T>& lFrame) const {
		return CFrameTemplate<T>(position + rotation.localToGlobal(lFrame.position), rotation.localToGlobal(lFrame.rotation));
	}

	inline CFrameTemplate<T> globalToLocal(const CFrameTemplate<T>& gFrame) const {
		return CFrameTemplate<T>(rotation.globalToLocal(gFrame.position - position), rotation.globalToLocal(gFrame.rotation));
	}
	
	inline CFrameTemplate<T> localToRelative(const CFrameTemplate<T>& lFrame) const {
		return CFrameTemplate<T>(rotation.localToGlobal(lFrame.position), rotation.localToGlobal(lFrame.rotation));
	}

	inline CFrameTemplate<T> relativeToLocal(const CFrameTemplate<T>& rFrame) const {
		return CFrameTemplate<T>(rotation.globalToLocal(rFrame.position), rotation.globalToLocal(rFrame.rotation));
	}

	inline RotationTemplate<T> localToGlobal(const RotationTemplate<T>& localRot) const {
		return rotation.localToGlobal(localRot);
	}

	inline RotationTemplate<T> globalToLocal(const RotationTemplate<T>& globalRot) const {
		return rotation.globalToLocal(globalRot);
	}

	inline CFrameTemplate<T> operator~() const {
		return CFrameTemplate<T>(rotation.globalToLocal(-position), ~rotation);
	}

	inline Vector<T, 3> getPosition() const {
		return position;
	}

	inline RotationTemplate<T> getRotation() const {
		return rotation;
	}

	inline CFrameTemplate<T>& operator+=(const Vector<T, 3>& delta) {
		position += delta;
		return *this;
	}

	inline CFrameTemplate<T>& operator-=(const Vector<T, 3>& delta) {
		position -= delta;
		return *this;
	}

	inline CFrameTemplate<T> operator+(const Vector<T, 3>& delta) const {
		return CFrameTemplate<T>(this->position + delta, this->rotation);
	}

	inline CFrameTemplate<T> operator-(const Vector<T, 3>& delta) const {
		return CFrameTemplate<T>(this->position - delta, this->rotation);
	}
};

typedef CFrameTemplate<double> CFrame;
typedef CFrameTemplate<float> CFramef;
