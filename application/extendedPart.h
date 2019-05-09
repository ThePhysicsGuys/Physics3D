#pragma once

#include <string>

#include "view/material.h"

#include "../engine/part.h"

struct ExtendedPart : public Part {
	
	Material material;

	std::string name;

	int renderMode = 0x1B02;

	int drawMeshId;

	ExtendedPart() = default;
	ExtendedPart(NormalizedShape shape, CFrame position, double density, double friction, int drawMeshId, std::string name = "Part");
	ExtendedPart(Shape shape, CFrame position, double density, double friction, int drawMeshId, std::string name = "Part");
};
