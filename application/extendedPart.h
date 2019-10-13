#pragma once

#include "view/material.h"

#include "../physics/part.h"

#include "visualShape.h"

struct ExtendedPart : public Part {
	
	Material material;

	std::string name;

	int renderMode = 0x1B02; // GL_FILL

	int drawMeshId;

	VisualShape visualShape;

	Mat3f visualScale = Mat3f::IDENTITY();

	ExtendedPart() = default;
	ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, int drawMeshId, std::string name = "Part");
	ExtendedPart(const VisualShape& shape, const GlobalCFrame& position, const PartProperties& properties, int drawMeshId, std::string name = "Part");
	ExtendedPart(const Shape& hitbox, const VisualShape& shape, const GlobalCFrame& position, const PartProperties& properties, int drawMeshId, std::string name = "Part");

	void scale(double scaleX, double scaleY, double scaleZ);
};
