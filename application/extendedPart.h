#pragma once

#include "view\material.h"
#include "../engine/part.h"

struct ExtendedPart : public Part {
	
	Material* material = nullptr;
	int drawMeshId;

	ExtendedPart() = default;
	ExtendedPart(NormalizedShape shape, CFrame position, double density, double friction, int drawMeshId);
	ExtendedPart(Shape shape, CFrame position, double density, double friction, int drawMeshId);

};
