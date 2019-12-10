#include "core.h"

#include "extendedPart.h"
#include "../physics/misc/serialization.h"
#include "view/screen.h"

ExtendedPart::ExtendedPart(Part&& part, VisualData visualData, std::string name) :
	Part(std::move(part)), visualData(visualData), name(name) {
}

ExtendedPart::ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, VisualData visualData, std::string name) :
	Part(hitbox, position, properties), visualData(visualData), name(name) {
}

ExtendedPart::ExtendedPart(Part&& part, std::string name) :
	Part(std::move(part)), visualData(Screen::getOrCreateMeshFor(part.hitbox.baseShape)), name(name) {
}
ExtendedPart::ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, std::string name) :
	Part(hitbox, position, properties), visualData(Screen::getOrCreateMeshFor(hitbox.baseShape)), name(name) {
}

ExtendedPart::ExtendedPart(const Shape& hitbox, ExtendedPart* attachTo, const CFrame& attach, const PartProperties& properties, std::string name) :
	Part(hitbox, *attachTo, attach, properties), visualData(Screen::getOrCreateMeshFor(hitbox.baseShape)), name(name) {
}
