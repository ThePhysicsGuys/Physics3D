#pragma once

#include "../engine/part.h"

struct ExtendedPart : public Part {
	int drawMeshId;

	ExtendedPart() = default;
	ExtendedPart(NormalizedShape shape, CFrame position, double density, double friction, int drawMeshId);
	ExtendedPart(Shape shape, CFrame position, double density, double friction, int drawMeshId);

};
