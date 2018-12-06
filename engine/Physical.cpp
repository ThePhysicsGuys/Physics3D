#include "physical.h"



Physical::Physical(Part p, CFrame cframe) : part(p), cframe(cframe) {}

void Physical::update(double deltaT) {
	cframe = cframe + velocity * deltaT;
}