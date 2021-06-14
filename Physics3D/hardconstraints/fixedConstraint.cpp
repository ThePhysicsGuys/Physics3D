#include "fixedConstraint.h"

namespace P3D {
void FixedConstraint::update(double deltaT) {}
void FixedConstraint::invert() {}
CFrame FixedConstraint::getRelativeCFrame() const { return CFrame(0.0, 0.0, 0.0); }
RelativeMotion FixedConstraint::getRelativeMotion() const { return RelativeMotion(Motion(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 0.0)), CFrame(0.0, 0.0, 0.0)); }
};