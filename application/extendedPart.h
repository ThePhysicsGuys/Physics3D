#pragma once

#include <string>

#include "view/material.h"

#include "../engine/part.h"

#include "visualShape.h"

struct ExtendedPart : public Part {
	
	Material material;

	std::string name;

	int renderMode = 0x1B02;

	int drawMeshId;

	VisualShape visualShape;

	ExtendedPart() = default;
	ExtendedPart(const Shape& hitbox, const CFrame& position, double density, double friction, int drawMeshId, std::string name = "Part");
	ExtendedPart(const VisualShape& shape, const CFrame& position, double density, double friction, int drawMeshId, std::string name = "Part");
	ExtendedPart(const Shape& hitbox, const VisualShape& shape, const CFrame& position, double density, double friction, int drawMeshId, std::string name = "Part");
};
