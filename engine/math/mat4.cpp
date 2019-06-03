#include "mat4.h"

#include <cmath>

template<typename N>
Mat4Template<N> Mat4Template<N>::rotate(N angle, N x, N y, N z) const {
	N s = sin(angle);
	N c = cos(angle);
	N C = (1 - c);

	N rm00 = x * x * C + c;
	N rm01 = x * y * C + z * s;
	N rm02 = x * z * C - y * s;
	N rm10 = x * y * C - z * s;
	N rm11 = y * y * C + c;
	N rm12 = y * z * C + x * s;
	N rm20 = x * z * C + y * s;
	N rm21 = y * z * C - x * s;
	N rm22 = z * z * C + c;
	N r00 = m00 * rm00 + m10 * rm01 + m20 * rm02;
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
	N r23 = m03 * rm20 + m13 * rm21 + m23 * rm22;

	return Mat4Template<N>(r00, r01, r02, r03, r10, r11, r12, r13, r20, r21, r22, r23, m30, m31, m32, m33);
}

template struct Mat4Template<double>;
template struct Mat4Template<float>;

/*template<typename N>
CFrame Mat4Template<N>::toCFrame() const {
	return CFrame(getTranslation(), getRotation());
}*/

Mat4f ortho(float left, float right, float bottom, float top, float zNear, float zFar) {
	float r00 = 2.0f / (right - left);
	float r11 = 2.0f / (top - bottom);
	float r22 = -2.0f / (zFar - zNear);
	float r30 = (left + right) / (left - right);
	float r31 = (top + bottom) / (bottom - top);
	float r32 = (zFar + zNear) / (zNear - zFar);
	float r33 = 1.0f;

	return Mat4f(r00, 0, 0, 0, 0, r11, 0, 0, 0, 0, r22, 0, r30, r31, r32, r33);
}

Mat4f perspective(float fov, float aspect, float zNear, float zFar) {
	float t = tan(fov / 2);
	float r00 = 1 / (t * aspect);
	float r11 = 1 / t;
	float r22 = (zFar + zNear) / (zNear - zFar);
	float r32 = (zFar + zFar) * zNear / (zNear - zFar);
	float r23 = -1;
	return Mat4f(r00, 0, 0, 0, 0, r11, 0, 0, 0, 0, r22, r23, 0, 0, r32, 0);
}


Mat4f lookAt(Vec3f from, Vec3f to, Vec3f up) {
	
	Vec3f z = -(from - to).normalize();
	Vec3f x = (up.normalize() % z);
	Vec3f y = z % x;

	return Mat4f (
		x.x, x.y, x.z, 0.0f,
		y.x, y.y, y.z, 0.0f,
		z.x, z.y, z.z, 0.0f,
		from.x, from.y, from.z, 1.0f
	);
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