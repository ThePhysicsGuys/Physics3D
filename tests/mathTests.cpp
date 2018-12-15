#include "testsMain.h"
#include "testUtils.h"

TEST_CASE(correctTest) {
	float a = 3.7;
	int b = 7;
	ASSERT(a <= b);
}
