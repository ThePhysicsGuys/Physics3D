#include "cframe.h"

Mat4 CFrame::asMat4() const {
	const Mat3& r = rotation;
	const Vec3& p = position;
	return Mat4(
		r.m00, r.m01, r.m02, 0,
		r.m10, r.m11, r.m12, 0,
		r.m20, r.m21, r.m22, 0,
		p.x,   p.y,   p.z,   1);
}

Mat4f CFrame::asMat4f() const {
	const Mat3f r = static_cast<Mat3f>(rotation);
	const Vec3f p = static_cast<Vec3f>(position);
	return Mat4f(
		r.m00, r.m01, r.m02, 0,
		r.m10, r.m11, r.m12, 0,
		r.m20, r.m21, r.m22, 0,
		p.x,   p.y,   p.z,   1);
}

CFrame& CFrame::operator+=(const Vec3& delta) { position += delta; return *this; }
CFrame& CFrame::operator-=(const Vec3& delta) { position -= delta; return *this; }

CFrame operator+(const CFrame& frame, const Vec3& delta) { return CFrame(frame.position + delta, frame.rotation); }
CFrame operator+(const Vec3& delta, const CFrame& frame) { return CFrame(frame.position + delta, frame.rotation); }
CFrame operator-(const CFrame& frame, const Vec3& delta) { return CFrame(frame.position - delta, frame.rotation); }
CFrame operator-(const Vec3& delta, const CFrame& frame) { return CFrame(frame.position - delta, frame.rotation); }

void CFrame::translate(Vec3 translation) {
	position += translation;
}
void CFrame::rotate(RotMat3 rot) {
	rotation = rot * rotation;
}
