#include "mat4.h"

#include <cmath>

template<typename N>
Mat4Template<N> Mat4Template<N>::rotate(N angle, N x, N y, N z) const {
	N s = sin(angle);
	N c = cos(angle);
	N C = (1.0 - c);

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

template<typename N>
Mat4Template<N> Mat4Template<N>::perspective(N fov, N aspect, N zNear, N zFar) const {
	N t = tan(fov / 2.0);
	N t00 = 1.0 / (t * aspect);
	N t11 = 1.0 / t;
	N t22 = (zFar + zNear) / (zNear - zFar);
	N t32 = (zFar + zFar) * zNear / (zNear - zFar);
	N r00 = m00 * t00;
	N r01 = m01 * t00;
	N r02 = m02 * t00;
	N r03 = m03 * t00;
	N r10 = m10 * t11;
	N r11 = m11 * t11;
	N r12 = m12 * t11;
	N r13 = m13 * t11;
	N r30 = m20 * t32;
	N r31 = m21 * t32;
	N r32 = m22 * t32;
	N r33 = m23 * t32;
	N r20 = m20 * t22 - m30;
	N r21 = m21 * t22 - m31;
	N r22 = m22 * t22 - m32;
	N r23 = m23 * t22 - m33;
	return Mat4Template(r00, r01, r02, r03, r10, r11, r12, r13, r20, r21, r22, r23, r30, r31, r32, r33);
}

template<typename N>
CFrame Mat4Template<N>::asCFrame() const {
	return CFrame(Vec3(m30, m31, m32), Mat3(m00, m01, m02, m10, m11, m12, m20, m21, m22));
};

template struct Mat4Template<double>;
template struct Mat4Template<float>;

/*template<typename N>
CFrame Mat4Template<N>::toCFrame() const {
	return CFrame(getTranslation(), getRotation());
}*/
