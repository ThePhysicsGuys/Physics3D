#pragma once

#include "linalg/vec.h"
#include "linalg/mat.h"

template<typename T>
struct CFrameTemplate {
public:
	Vector<T, 3> position;
	Matrix<T, 3, 3> rotation;

	CFrameTemplate(Vector<T, 3> position, Matrix<T, 3, 3> rotation) : position(position), rotation(rotation) {}
	explicit CFrameTemplate(Vector<T, 3> position) : position(position), rotation(Mat3::IDENTITY()) {}
	explicit CFrameTemplate(T x, T y, T z) : position(x, y, z), rotation(Mat3::IDENTITY()) {}
	explicit CFrameTemplate(Matrix<T, 3, 3> rotation) : position(0,0,0), rotation(rotation) {}
	CFrameTemplate() : position(0, 0, 0), rotation(Mat3::IDENTITY()) {}

	template<typename OtherT>
	CFrameTemplate(const CFrameTemplate<OtherT>& other) : position(Vector<T, 3>(other.position)), rotation(Matrix<T, 3, 3>(other.rotation)) {}

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

	inline Matrix<T, 3, 3> localToGlobal(const Matrix<T, 3, 3>& localRot) const {
		return rotation * localRot;
	}

	inline Matrix<T, 3, 3> globalToLocal(const Matrix<T, 3, 3>& globalRot) const {
		return rotation.transpose() * globalRot;
	}

	inline CFrameTemplate<T> operator~() const {
		return CFrameTemplate<T>(rotation.transpose() * -position, rotation.transpose());
	}

	inline Vector<T, 3> getPosition() const {
		return position;
	}

	inline Matrix<T, 3, 3> getRotation() const {
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

	void rotate(Matrix<T, 3, 3> rot) {
		rotation = rot * rotation;
	}
};

template<typename T>
CFrameTemplate<T> Mat4ToCFrame(const Matrix<T, 4, 4>& mat) {
	return CFrameTemplate<T>(Vector<T, 3>(mat[0][3], mat[1][3], mat[2][3]), mat.getSubMatrix<3, 3>(0,0));
};

template<typename T>
Matrix<T, 4, 4> CFrameToMat4(const CFrameTemplate<T>& cframe) {
	const Matrix<T, 3, 3>& r = cframe.rotation;
	const Vector<T, 3>& p = cframe.position;
	return Matrix<T, 4, 4>(Matrix<T, 3, 3>(r), p, Vector<T, 3>(0, 0, 0), 1.0);
}

template<typename T> CFrameTemplate<T> operator+(const CFrameTemplate<T>& frame, const Vector<T, 3>& delta) {
	return CFrameTemplate<T>(frame.position + delta, frame.rotation); 
}

template<typename T> CFrameTemplate<T> operator-(const CFrameTemplate<T>& frame, const Vector<T, 3>& delta) { 
	return CFrameTemplate<T>(frame.position - delta, frame.rotation); 
}

typedef CFrameTemplate<double> CFrame;
typedef CFrameTemplate<float> CFramef;
