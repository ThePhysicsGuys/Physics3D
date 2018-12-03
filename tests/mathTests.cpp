#include "testsMain.h"
#include "testUtils.h"

TEST_CASE(basicTest) {
	float a = 3.7;
	int b = 7;
	ASSERT(a >= b);
}
TEST_CASE(corectTest) {
	float a = 3.7;
	int b = 7;
	ASSERT(a <= b);
}
