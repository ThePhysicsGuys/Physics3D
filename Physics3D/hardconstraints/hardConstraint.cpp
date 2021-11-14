#include "hardConstraint.h"

namespace P3D {
CFrame HardConstraint::getRelativeCFrame() const {
	return this->getRelativeMotion().locationOfRelativeMotion;
}
HardConstraint::~HardConstraint() {}
};