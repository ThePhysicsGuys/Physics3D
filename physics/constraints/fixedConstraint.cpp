#include "fixedConstraint.h"

void FixedConstraint::update(double deltaT) {}
void FixedConstraint::invert() {}
// this CFrame determines the relative position of the constraint's attach1 and attach2
CFrame FixedConstraint::getRelativeCFrame() { return CFrame(0.0,0.0,0.0); }

Motion FixedConstraint::getRelativeMotion() { return Motion(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 0.0)); }

