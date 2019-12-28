#pragma once

#include "../graphics/visualShape.h"
#include "../physics/geometry/normalizedPolyhedron.h"
#include "extendedPart.h"
#include "view/screen.h"

namespace Application {

class PartFactory {
	mutable size_t count = 0;

	std::string name;
	Shape hitbox;
	VisualData visualData;

public:
	PartFactory() = default;
	PartFactory(const Shape& hitbox, Screen& screen, std::string name = "Part");
	PartFactory(const Shape& hitbox, const VisualShape& visualShape, Screen& screen, std::string name = "Part");
	ExtendedPart* produce(const GlobalCFrame& cframe, const PartProperties& properties, std::string name = "Part") const;
	ExtendedPart* produce(const PartProperties& properties, std::string name = "Part") const;
	ExtendedPart* produce(ExtendedPart* partToAttachTo, const CFrame& attachment, const PartProperties& properties, std::string name = "Part") const;
	ExtendedPart* produceScaled(const GlobalCFrame& cframe, const PartProperties& properties, double scaleX, double scaleY, double scaleZ, std::string name = "Part") const;
	ExtendedPart* produceScaled(const PartProperties& properties, double scaleX, double scaleY, double scaleZ, std::string name = "Part") const;
	ExtendedPart* produceScaled(ExtendedPart* partToAttachTo, const CFrame& attachment, const PartProperties& properties, double scaleX, double scaleY, double scaleZ, std::string name = "Part") const;
};

};
