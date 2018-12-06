
#include "world.h"

#include <iostream>

#include "../util/Log.h"

World::World() {

}

void World::tick(double deltaT) {
	// Log::debug("Tick Tock Time: %.9f", deltaT);

	// Dummy load
	/*double d = 5.2;
	for (int i = 0; i < 20000000; i++)
		d = d / 1.2 - 3.2;
	
	Log::info("Dummy Load: %.9f", d);*/

	Log::info("Ticking!");

	for (Physical& p : physicals) {
		p.update(deltaT);
	}
}

void World::addObject(Physical p) {
	physicals.push_back(p);
}
