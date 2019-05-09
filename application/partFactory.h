#pragma once

#include <string>

#include "extendedPart.h"
#include "view/screen.h"

class PartFactory {
	mutable size_t count;
	
	std::string name;
	NormalizedShape normalizedShape;
	int drawMeshID;
	CFrame backTransform;
public:
	PartFactory() = default;
	PartFactory(const Shape& shape, Screen& screen, std::string name = "Part");
	PartFactory(const NormalizedShape& shape, Screen& screen, std::string name = "Part");
	ExtendedPart* produce(const CFrame& cframe, double density, double friction, std::string name = "") const;
};

ExtendedPart* createUniquePart(Screen& screen, const NormalizedShape& shape, CFrame position, double density, double friction, std::string name = "Part");
ExtendedPart* createUniquePart(Screen& screen, const Shape& shape, CFrame position, double density, double friction, std::string name = "Part");
