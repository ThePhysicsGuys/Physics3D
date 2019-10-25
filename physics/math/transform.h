#pragma once

#include "linalg/vec.h"
#include "linalg/mat.h"
#include "cframe.h"

template<typename T>
class TransformTemplate {
public:
	Vector<T, 3> position;
	Matrix<T, 3, 3> transform;

	TransformTemplate(const Vector<T, 3>& position, const Matrix<T, 3, 3>& transform) : position(position), transform(transform) {}
	explicit TransformTemplate(const Vector<T, 3>& position) : position(position), transform(Mat3::IDENTITY()) {}
	explicit TransformTemplate(const T& x, const T& y, const T& z) : position(x, y, z), transform(Mat3::IDENTITY()) {}
	explicit TransformTemplate(const Matrix<T, 3, 3>& transform) : position(0, 0, 0), transform(transform) {}
	TransformTemplate() : position(0, 0, 0), transform(Mat3::IDENTITY()) {}

	template<typename OtherT>
	TransformTemplate(const TransformTemplate<OtherT>& other) : 
		position(Vector<T, 3>(other.position)),
		transform(Matrix<T, 3, 3>(other.transform)) {}

	template<typename OtherT>
	TransformTemplate(const CFrameTemplate<OtherT>& other) : 
		position(Vector<T, 3>(other.position)),
		transform(Matrix<T, 3, 3>(other.rotation)) {}


	inline Vector<T, 3> localToGlobal(const Vector<T, 3>& lVec) const {
		return transform * lVec + position;
	}

	inline Vector<T, 3> globalToLocal(const Vector<T, 3>& gVec) const {
		return ~transform * (gVec - position);
	}

	inline Vector<T, 3> localToRelative(const Vector<T, 3>& lVec) const {
		return transform * lVec;
	}

	inline Vector<T, 3> relativeToLocal(const Vector<T, 3>& rVec) const {
		return ~transform * rVec;
	}

	inline TransformTemplate<T> localToGlobal(TransformTemplate<T> lFrame) const {
		return TransformTemplate<T>(position + transform * lFrame.position, transform * lFrame.transform);
	}

	inline TransformTemplate<T> globalToLocal(const TransformTemplate<T>& gFrame) const {
		return TransformTemplate<T>(~transform * (gFrame.position - position), ~transform * gFrame.transform);
	}

	inline TransformTemplate<T> localToRelative(const TransformTemplate<T>& lFrame) const {
		return TransformTemplate<T>(transform * lFrame.position, transform * lFrame.transform);
	}

	inline TransformTemplate<T> relativeToLocal(const TransformTemplate<T>& rFrame) const {
		return TransformTemplate<T>(~transform * rFrame.position, ~transform * rFrame.transform);
	}

	inline Matrix<T, 3, 3> localToGlobal(const Matrix<T, 3, 3>& localRot) const {
		return transform * localRot;
	}

	inline Matrix<T, 3, 3> globalToLocal(const Matrix<T, 3, 3>& globalRot) const {
		return ~transform * globalRot;
	}

	inline TransformTemplate<T> operator~() const {
		return TransformTemplate<T>(~transform * -position, ~transform);
	}

	inline Vector<T, 3> getPosition() const {
		return position;
	}

	inline Matrix<T, 3, 3> getLocalTransformation() const {
		return transform;
	}

	inline TransformTemplate<T>& operator+=(const Vector<T, 3>& delta) {
		position += delta;
		return *this;
	}

	inline TransformTemplate<T>& operator-=(const Vector<T, 3>& delta) {
		position -= delta;
		return *this;
	}

	void translate(const Vector<T, 3>& translation) {
		position += translation;
	}

	void transformGlobalSide(const Matrix<T, 3, 3>& transformation) {
		transform = transformation * transform;
	}

	void transformLocalSide(const Matrix<T, 3, 3>& transformation) {
		transform = transform * transformation;
	}
};


template<typename T>
TransformTemplate<T> Mat4ToTransform(const Matrix<T, 4, 4>& mat) {
	return TransformTemplate<T>(toVector(mat.getSubMatrix<1, 3>(0, 3)), mat.getSubMatrix<3, 3>(0, 0));
	assert(mat[3][0] == 0.0);
	assert(mat[3][1] == 0.0);
	assert(mat[3][2] == 0.0);
	assert(mat[3][3] == 1.0);
};
template<typename T>
Matrix<T, 4, 4> TransformToMat4(const TransformTemplate<T>& transform) {
	return Matrix<T, 4, 4>(Matrix<T, 3, 3>(transform.transform), transform.position, Vector<T, 3>(0, 0, 0), 1.0);
}

typedef TransformTemplate<double> Transform;
typedef TransformTemplate<float> Transformf;
