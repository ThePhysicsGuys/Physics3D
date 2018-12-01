#include "cframe.h"

Mat4 CFrame::asMat4() const {
	const Mat3& r = rotation;
	const Vec3& p = position;
	return Mat4(r.m00, r.m01, r.m02, p.x,
				r.m10, r.m11, r.m12, p.y,
				r.m20, r.m21, r.m22, p.z,
				0, 0, 0, 1);
}
