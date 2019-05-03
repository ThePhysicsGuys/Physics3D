#pragma once

#include "extendedPart.h"
#include "view/screen.h"

class PartFactory {
	NormalizedShape normalizedShape;
	int drawMeshID;
	CFrame backTransform;
public:
	PartFactory() = default;
	PartFactory(const Shape& shape, Screen& screen);
	PartFactory(const NormalizedShape& shape, Screen& screen);
	ExtendedPart* produce(const CFrame& cframe, double density, double friction) const;
};

ExtendedPart* createUniquePart(Screen& screen, const NormalizedShape& shape, CFrame position, double density, double friction);
ExtendedPart* createUniquePart(Screen& screen, const Shape& shape, CFrame position, double density, double friction);
