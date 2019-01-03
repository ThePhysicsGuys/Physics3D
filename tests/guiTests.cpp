#include "../engine/math/mathUtil.h"

#include "testsMain.h"

#include "../application/gui/loader.h"
#include "../engine/math/mat4.h"
#include "../engine/math/vec4.h"
#include "../engine/math/vec3.h"
#include "../engine/math/vec2.h"

#include <string>
#include <vector>

std::vector<std::string> split(std::string &string, char splitter) {
	std::vector<std::string> elements;
	int length = string.length();
	int start = 0;
	for (int i = 0; i < length; i++) {
		if (string[i] == splitter) {
			if (i != start) elements.push_back(string.substr(start, i - start));
			start = i + 1;
		}
	}
	if (start < length) elements.push_back(string.substr(start, length - start));
	return elements;
}

TEST_CASE(splitTest) {
	std::string testString = "/this is/a test /";
	std::vector<std::string> testVector = split(testString, '/');
	for (std::string s : testVector) {
		Log::fatal("%s", s.c_str());
	}
}

TEST_CASE(transformationTest) {
	Mat4f viewMatrix = Mat4f();
	Mat4f projectionMatrix = Mat4f().perspective(1, 2, 0.1, 100);

	Mat4f inverseViewMatrix = viewMatrix.inverse();
	Mat4f inverseProjectionMatrix = projectionMatrix.inverse();

	Vec4f point = Vec4f(1.0, 2.0, 3.0, 1.0);
	Vec4f transformedPoint = projectionMatrix * viewMatrix * point;
	Vec4f transformedBack = inverseViewMatrix * inverseProjectionMatrix * transformedPoint;

	ASSERT_TOLERANT(point == transformedBack, 0.00000001);
}
