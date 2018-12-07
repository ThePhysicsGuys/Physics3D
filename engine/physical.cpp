#include "physical.h"



Physical::Physical(Part p, CFrame cframe) : part(p), cframe(cframe) {}

void Physical::update(double deltaT) {
	cframe = cframe + velocity * deltaT;
	cframe.rotation = fromEulerAngles(0.01, 0.0, 0.0) * cframe.rotation;
}
