#pragma once

#include <memory>

#include "hardConstraint.h"

#include "../math/cframe.h"
#include "../relativeMotion.h"

namespace P3D {
struct HardPhysicalConnection {
	CFrame attachOnChild;
	CFrame attachOnParent;
	std::unique_ptr<HardConstraint> constraintWithParent;

	HardPhysicalConnection(std::unique_ptr<HardConstraint> constraintWithParent, const CFrame& attachOnChild, const CFrame& attachOnParent);

	CFrame getRelativeCFrameToParent() const;
	RelativeMotion getRelativeMotion() const;

	void update(double deltaT);
	HardPhysicalConnection inverted()&&;
};
};