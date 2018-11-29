
#include <catch.h>
#include "testUtils.h"
#include <iostream>
#include "../engine/math/vec3.h"
#include "../engine/math/mat3.h"
#include "../engine/math/cframe.h"
#include "../util/Log.h"

TEST_CASE("Vec3 computation", "[MathTests]") {
	Vec3 v1(0.1, 0.1, 0.2);
	Vec3 v2(0.2, 0.3, 0.4);

	Vec3 x1(1.0, 0.0, 0.0);
	Vec3 y1(0.0, 1.0, 0.0);
	Vec3 z1(0.0, 0.0, 1.0);

	Vec3 sum = v1 + v2;

	REQUIRE_VEC(sum, Vec3(0.3, 0.4, 0.6));

	Vec3 cr = v1 % v2;

	Log::debug("cr: %.9f, %0.9f, %0.9f", cr.x, cr.y, cr.z);

	REQUIRE_VEC((x1 % y1), z1);
	REQUIRE_VEC((y1 % x1), -z1);
	REQUIRE_VEC((y1 % z1), x1);
	REQUIRE_VEC((z1 % y1), -x1);
	REQUIRE_VEC((z1 % x1), y1);
	REQUIRE_VEC((x1 % z1), -y1);

	REQUIRE_ZERO(cr * v1);
	REQUIRE_ZERO(cr * v2);

	REQUIRE_VEC_ZERO((cr % cr));
}

TEST_CASE("Mat3 computation", "[MathTests]") {
	Mat3 mz = fromEulerAngles(0.3, 0.0, 0.0);
	Mat3 mx = fromEulerAngles(0.0, 0.5, 0.0);
	Mat3 my = fromEulerAngles(0.0, 0.0, 0.7);

	std::cout << "mz: " << mz << "\n";
	std::cout << "mx: " << mx << "\n";
	std::cout << "my: " << my << "\n";
}