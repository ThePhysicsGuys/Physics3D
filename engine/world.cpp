
#include "world.h"

#include <iostream>

#include "../util/Log.h"

World::World() {

}

void World::tick(double deltaT) {
	char buf[31];
	sprintf_s(buf, "Tick Tock Time: %.9f", deltaT);
	Log::info(buf);


	// Dummy load
	double d = 5.2;
	for (int i = 0; i < 50000000; i++) {
		d = d / 1.2 - 3.2;
	}
	char fuf[50];
	sprintf_s(fuf, "%.9f", d);
	Log::info(fuf);

}
