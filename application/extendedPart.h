#pragma once

#include <string>

#include "view/material.h"

#include "../engine/part.h"

#include "visualShape.h"

struct ExtendedPart : public Part {
	
	Material material;

	std::string name;

	int renderMode = 0x1B02; // GL_FILL

	int drawMeshId;

	VisualShape visualShape;

	Mat3f visualScale = Mat3f(1, 0, 0, 
							  0, 1, 0, 
							  0, 0, 1);

	ExtendedPart() = default;
	ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, double density, double friction, int drawMeshId, std::string name = "Part");
	ExtendedPart(const VisualShape& shape, const GlobalCFrame& position, double density, double friction, int drawMeshId, std::string name = "Part");
	ExtendedPart(const Shape& hitbox, const VisualShape& shape, const GlobalCFrame& position, double density, double friction, int drawMeshId, std::string name = "Part");

	void scale(double scaleX, double scaleY, double scaleZ);
};
