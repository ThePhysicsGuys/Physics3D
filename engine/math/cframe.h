#pragma once

#include "vec.h"
#include "mat3.h"
#include "mat4.h"

template<typename T>
struct CFrameTemplate {
public:
	Vector<T, 3> position;
	Mat3Template<T> rotation;

	CFrameTemplate(Vector<T, 3> position, Mat3Template<T> rotation) : position(position), rotation(rotation) {}
	explicit CFrameTemplate(Vector<T, 3> position) : position(position), rotation(1, 0, 0, 0, 1, 0, 0, 0, 1) {}
	explicit CFrameTemplate(T x, T y, T z) : position(x, y, z), rotation(1, 0, 0, 0, 1, 0, 0, 0, 1) {}
	explicit CFrameTemplate(Mat3Template<T> rotation) : position(0,0,0), rotation(rotation) {}
	CFrameTemplate() : position(0, 0, 0), rotation(1, 0, 0, 0, 1, 0, 0, 0, 1) {}

	template<typename OtherT>
	CFrameTemplate(const CFrameTemplate<OtherT>& other) : position(Vector<T, 3>(other.position)), rotation(Mat3Template<T>(other.rotation)) {}

	inline Vector<T, 3> localToGlobal(Vector<T, 3> lVec) const {
		return rotation * lVec + position;
	}

	inline Vector<T, 3> globalToLocal(Vector<T, 3> gVec) const {
		return rotation.transpose() * (gVec - position);
	}

	inline Vector<T, 3> localToRelative(Vector<T, 3> lVec) const {
		return rotation * lVec;
	}

	inline Vector<T, 3> relativeToLocal(Vector<T, 3> rVec) const {
		return rotation.transpose() * rVec;
	}

	inline CFrameTemplate<T> localToGlobal(CFrameTemplate<T> lFrame) const {
		return CFrameTemplate<T>(position + rotation * lFrame.position, rotation * lFrame.rotation);
	}

	inline CFrameTemplate<T> globalToLocal(CFrameTemplate<T> gFrame) const {
		return CFrameTemplate<T>(rotation.transpose()*(gFrame.position - position) , rotation.transpose() * gFrame.rotation);
	}
	
	inline CFrameTemplate<T> localToRelative(CFrameTemplate<T> lFrame) const {
		return CFrameTemplate<T>(rotation * lFrame.position, rotation * lFrame.rotation);
	}

	inline CFrameTemplate<T> relativeToLocal(CFrameTemplate<T> rFrame) const {
		return CFrameTemplate<T>(rotation.transpose()*rFrame.position, rotation.transpose() * rFrame.rotation);
	}

	inline Mat3Template<T> localToGlobal(const Mat3Template<T>& localRot) const {
		return rotation * localRot;
	}

	inline Mat3Template<T> globalToLocal(const Mat3Template<T>& globalRot) const {
		return rotation.transpose() * globalRot;
	}

	inline CFrameTemplate<T> operator~() const {
		return CFrameTemplate<T>(rotation.transpose() * -position, rotation.transpose());
	}

	inline Vector<T, 3> getPosition() const {
		return position;
	}

	inline Mat3Template<T> getRotation() const {
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

	void translate(Vector<T, 3> translation) {
		position += translation;
	}

	void rotate(Mat3Template<T> rot) {
		rotation = rot * rotation;
	}
};

template<typename T>
CFrameTemplate<T> Mat4ToCFrame(const Mat4Template<T>& mat) {
	return CFrameTemplate<T>(Vector<T, 3>(mat.m30, mat.m31, mat.m32), Mat3Template<T>(mat.m00, mat.m01, mat.m02, mat.m10, mat.m11, mat.m12, mat.m20, mat.m21, mat.m22));
};

template<typename T>
Mat4Template<T> CFrameToMat4(const CFrameTemplate<T>& cframe) {
	const Mat3Template<T>& r = cframe.rotation;
	const Vector<T, 3>& p = cframe.position;
	return Mat4Template<T> (
		r.m00, r.m01, r.m02, 0,
		r.m10, r.m11, r.m12, 0,
		r.m20, r.m21, r.m22, 0,
		p.x,   p.y,   p.z,   1
	);
}

template<typename T> CFrameTemplate<T> operator+(const CFrameTemplate<T>& frame, const Vector<T, 3>& delta) {
	return CFrameTemplate<T>(frame.position + delta, frame.rotation); 
}

template<typename T> CFrameTemplate<T> operator-(const CFrameTemplate<T>& frame, const Vector<T, 3>& delta) { 
	return CFrameTemplate<T>(frame.position - delta, frame.rotation); 
}

typedef CFrameTemplate<double> CFrame;
typedef CFrameTemplate<float> CFramef;
