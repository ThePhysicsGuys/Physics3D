#include "testsMain.h"

#include "compare.h"
#include <Physics3D/misc/toString.h>

#include <Physics3D/math/linalg/mat.h>
#include <Physics3D/math/linalg/vec.h>

#include <Physics3D/math/mathUtil.h>
#include <Physics3D/math/linalg/trigonometry.h>

#include <string>
#include <vector>

using namespace P3D;
TEST_CASE(transformationTest) {
	Mat4f viewMatrix = Mat4f::IDENTITY();
	Mat4f projectionMatrix = perspective(1, 2, 0.1f, 100);

	Mat4f inverseViewMatrix = ~viewMatrix;
	Mat4f inverseProjectionMatrix = ~projectionMatrix;

	Vec4f point = Vec4f(1.0, 2.0, 3.0, 1.0);
	Vec4f transformedPoint = projectionMatrix * viewMatrix * point;
	Vec4f transformedBack = inverseViewMatrix * inverseProjectionMatrix * transformedPoint;

	ASSERT_TOLERANT(point == transformedBack, 0.000001);
}
