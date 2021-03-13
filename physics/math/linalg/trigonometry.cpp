#include "trigonometry.h"
#include "mat.h"

#include <cmath>

template<typename T>
Matrix<T, 4, 4> rotate(const Matrix<T, 4, 4>& mat, T angle, T x, T y, T z) {
	T s = sin(angle);
	T c = cos(angle);
	T C = (1 - c);

	Matrix<T, 3, 3> rotator{
		x * x * C + c,			x * y * C - z * s,		x * z * C + y * s,
		y * x * C + z * s,		y * y * C + c,			y * z * C - x * s,
		z * x * C - y * s,		z * y * C + x * s,		z * z * C + c
	};

	Matrix<T, 4, 3> leftSide = mat.template getSubMatrix<4, 3>(0, 0);

	Matrix<T, 4, 3> rotated = leftSide * rotator;

	Matrix<T, 4, 1> translation = mat.template getSubMatrix<4, 1>(0, 3);

	return joinHorizontal(rotated, translation);
}

Mat4f ortho(float left, float right, float bottom, float top, float zNear, float zFar) {
	float r00 = 2.0f / (right - left);
	float r11 = 2.0f / (top - bottom);
	float r22 = -2.0f / (zFar - zNear);
	float r30 = (left + right) / (left - right);
	float r31 = (top + bottom) / (bottom - top);
	float r32 = (zFar + zNear) / (zNear - zFar);
	float r33 = 1.0f;

	return Mat4f{
		r00, 0, 0, r30,
		0, r11, 0, r31,
		0, 0, r22, r32,
		0, 0, 0,   r33
	};
}

Mat4f perspective(float fov, float aspect, float zNear, float zFar) {
	float t = tan(fov / 2);
	float r00 = 1 / (t * aspect);
	float r11 = 1 / t;
	float r22 = (zFar + zNear) / (zNear - zFar);
	float r32 = (zFar + zFar) * zNear / (zNear - zFar);
	float r23 = -1;
	return Mat4f{
		r00, 0, 0, 0,
		0, r11, 0, 0,
		0, 0, r22, r32,
		0, 0, r23, 0
	};
}

Mat4f lookAt(const Vec3f& from, const Vec3f& to, const Vec3f& up) {
	
	/*Vec3f z = normalize(to - from);
	Vec3f x = normalize(up) % z;
	Vec3f y = z % x;

	return Mat4f {
		x.x, x.y, x.z, -from.x,
		y.x, y.y, y.z, -from.y,
		-z.x, -z.y, -z.z, from.z,
		0.0f, 0.0f, 0.0f, 1.0f
	};*/

	Vec3f f = normalize(to - from);
	Vec3f u = normalize(up);
	Vec3f s = normalize(f % u);
	u = s % f;

	return Mat4f {
		 s.x,  s.y,  s.z, -dot(s, from),
		 u.x,  u.y,  u.z, -dot(u, from),
		-f.x, -f.y, -f.z,  dot(f, from),
		 0.0f, 0.0f, 0.0f, 1.0f
	};
}
