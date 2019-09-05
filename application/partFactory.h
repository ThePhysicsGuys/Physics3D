#pragma once

#include "visualShape.h"
#include "extendedPart.h"
#include "view/screen.h"

class PartFactory {
	mutable size_t count;
	
	std::string name;
	Shape hitbox;
	VisualShape visualShape;
	int drawMeshID;
	CFramef backTransform;
public:
	PartFactory() = default;
	PartFactory(const Shape& hitbox, const VisualShape& visualShape, Screen& screen, std::string name);
	PartFactory(const Shape& shape, Screen& screen, std::string name = "Part") : PartFactory(shape, VisualShape(shape), screen, name) {}
	PartFactory(const VisualShape& shape, Screen& screen, std::string name) : PartFactory(static_cast<const Shape&>(shape), shape, screen, name) {}
	ExtendedPart* produce(const GlobalCFrame& cframe, double density, double friction, std::string name = "") const;
	ExtendedPart* produceScaled(const GlobalCFrame& cframe, double density, double friction, double scaleX, double scaleY, double scaleZ, std::string name = "") const;
};

ExtendedPart* createUniquePart(Screen& screen, const Shape& hitbox, const VisualShape& visualShape, const GlobalCFrame& position, double density, double friction, std::string name = "Part");

inline ExtendedPart* createUniquePart(Screen& screen, const Shape& shape, const GlobalCFrame& position, double density, double friction, std::string name = "Part") {
	return createUniquePart(screen, shape, VisualShape(shape), position, density, friction, name);
}

inline ExtendedPart* createUniquePart(Screen& screen, const VisualShape& shape, const GlobalCFrame& position, double density, double friction, std::string name = "Part") {
	return createUniquePart(screen, static_cast<const Shape&>(shape), shape, position, density, friction, name);
}
