#include "physical.h"


#include "../util/log.h"
#include "math/mathUtil.h"

Physical::Physical(Part p, CFrame cframe) : part(p), cframe(cframe) {}

void Physical::update(double deltaT) {
	cframe = cframe + velocity * deltaT;
	
	Mat3 rotation = fromRotationVec(angularVelocity * deltaT);

	Log::debug("Rotation: %s", str(rotation).c_str());

	angularVelocity += Vec3(0.001, 0.0, 0.0);

	cframe.rotation = rotation * cframe.rotation;
}
