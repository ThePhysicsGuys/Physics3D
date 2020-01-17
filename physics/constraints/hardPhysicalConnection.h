#pragma once

#include <memory>

#include "../math/cframe.h"
#include "hardConstraint.h"


struct HardPhysicalConnection {
	CFrame attachOnChild;
	CFrame attachOnParent;
	std::unique_ptr<HardConstraint> constraintWithParent;

	HardPhysicalConnection(const CFrame& attachOnChild, const CFrame& attachOnParent, std::unique_ptr<HardConstraint> constraintWithParent);

	CFrame getRelativeCFrameToParent() const;
	void update(double deltaT);
	HardPhysicalConnection inverted() &&;
};
