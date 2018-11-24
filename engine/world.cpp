
#include "world.h"

#include <iostream>

#include "../util/Log.h"

World::World() {

}

void World::tick(double deltaT) {
	char buf[31];
	sprintf_s(buf, "Tick Tock Time: %.9f", deltaT);
	Log::info(buf);
}
