
#include "world.h";

#include <iostream>

World::World() {

}

void World::tick(double deltaT) {
	printf("Tick Tock Time: %.9f", deltaT);
}
