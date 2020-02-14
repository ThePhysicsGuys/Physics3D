#include "hardConstraint.h"

CFrame HardConstraint::getRelativeCFrame() const {
	return this->getRelativeMotion().locationOfRelativeMotion;
}
