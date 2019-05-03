#include "../engine/math/mathUtil.h"

#include "testsMain.h"

#include "../application/gui/loader.h"
#include "../engine/math/mat4.h"
#include "../engine/math/vec4.h"
#include "../engine/math/vec3.h"
#include "../engine/math/vec2.h"

#include <string>
#include <vector>

TEST_CASE(transformationTest) {
	Mat4f viewMatrix = Mat4f();
	Mat4f projectionMatrix = perspective(1, 2, 0.1f, 100);

	Mat4f inverseViewMatrix = viewMatrix.inverse();
	Mat4f inverseProjectionMatrix = projectionMatrix.inverse();

	Vec4f point = Vec4f(1.0, 2.0, 3.0, 1.0);
	Vec4f transformedPoint = projectionMatrix * viewMatrix * point;
	Vec4f transformedBack = inverseViewMatrix * inverseProjectionMatrix * transformedPoint;

	ASSERT_TOLERANT(point == transformedBack, 0.000001);
}
