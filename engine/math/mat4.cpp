#include "mat4.h"

#include <cmath>

template<typename N>
Matrix<N, 4, 4> rotate(const Matrix<N, 4, 4>& mat, N angle, N x, N y, N z) {
	N s = sin(angle);
	N c = cos(angle);
	N C = (1 - c);

	Matrix<N, 3, 3> rotator{
		x * x * C + c,			x * y * C - z * s,		x * z * C + y * s,
		y * x * C + z * s,		y * y * C + c,			y * z * C - x * s,
		z * x * C - y * s,		z * y * C + x * s,		z * z * C + c
	};

	Matrix<3, 4> leftSide = mat.getSubMatrix<3, 4>(0, 0);

	Matrix<3, 4> rotated = leftSide * rotator;

	Matrix<1, 4> translation = mat.getSubMatrix<1, 4>(0, 3);

	/*N r00 = m00 * rm00 + m10 * rm01 + m20 * rm02;
	N r01 = m01 * rm00 + m11 * rm01 + m21 * rm02;
	N r02 = m02 * rm00 + m12 * rm01 + m22 * rm02;
	N r03 = m03 * rm00 + m13 * rm01 + m23 * rm02;
	N r10 = m00 * rm10 + m10 * rm11 + m20 * rm12;
	N r11 = m01 * rm10 + m11 * rm11 + m21 * rm12;
	N r12 = m02 * rm10 + m12 * rm11 + m22 * rm12;
	N r13 = m03 * rm10 + m13 * rm11 + m23 * rm12;
	N r20 = m00 * rm20 + m10 * rm21 + m20 * rm22;
	N r21 = m01 * rm20 + m11 * rm21 + m21 * rm22;
	N r22 = m02 * rm20 + m12 * rm21 + m22 * rm22;
	N r23 = m03 * rm20 + m13 * rm21 + m23 * rm22;*/

	return joinHorizontal(rotated, translation);/*Matrix<N, 4, 4>{
		r00, r10, r20, r30, 
		r01, r11, r21, r31,
		r02, r12, r22, r32,
		m03, m13, m23, m33
	};*/
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
		0, 0, 0, r33
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


Mat4f lookAt(Vec3f from, Vec3f to, Vec3f up) {
	
	Vec3f z = -normalize(from - to);
	Vec3f x = (normalize(up) % z);
	Vec3f y = z % x;

	return Mat4f{
		x.x, y.x, z.x, from.x,
		x.y, y.y, z.y, from.y,
		x.z, y.z, z.z, from.z,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

/*Mat4f lookAt(Vec3f from, Vec3f to, Vec3f up) {
	Vec3f z = -(to - from).normalize();
	Vec3f x = (up.normalize() % z);
	Vec3f y = z % x;

	return Mat4f(
		x.x, y.x, z.x, 0.0f,
		x.y, y.y, z.y, 0.0f,
		z.x, z.y, z.z, 0.0f,
		from.x, from.y, from.z, 1.0f
	).transpose();
}*/

/*	return Mat4f(
		x.x, y.x, z.x, 0.0f,
		x.y, y.y, z.y, 0.0f,
		z.x, z.y, z.z, 0.0f,
		from.x, from.y, from.z, .0f
	);*/