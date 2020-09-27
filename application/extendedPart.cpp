#include "core.h"

#include "extendedPart.h"
#include "../physics/misc/serialization.h"
#include "view/screen.h"

#include "../engine/meshRegistry.h"

namespace P3D::Application {

ExtendedPart::ExtendedPart(Part&& part, VisualData visualData, std::string name) :
	Part(std::move(part)), visualData(visualData), name(name) {
}

ExtendedPart::ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, VisualData visualData, std::string name) :
	Part(hitbox, position, properties), visualData(visualData), name(name) {
}

ExtendedPart::ExtendedPart(Part&& part, std::string name) :
	Part(std::move(part)), visualData(Engine::MeshRegistry::getOrCreateMeshFor(part.hitbox.baseShape)), name(name) {
}
ExtendedPart::ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, std::string name) :
	Part(hitbox, position, properties), visualData(Engine::MeshRegistry::getOrCreateMeshFor(hitbox.baseShape)), name(name) {
}

ExtendedPart::ExtendedPart(const Shape& hitbox, ExtendedPart* attachTo, const CFrame& attach, const PartProperties& properties, std::string name) :
	Part(hitbox, *attachTo, attach, properties), visualData(Engine::MeshRegistry::getOrCreateMeshFor(hitbox.baseShape)), name(name) {
}

// Transition
ExtendedPart::ExtendedPart(Part&& part, Entity entity) : Part(std::move(part)), entity(entity) {}
ExtendedPart::ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, Entity entity) : Part(hitbox, position, properties), entity(entity) {}
ExtendedPart::ExtendedPart(const Shape& hitbox, ExtendedPart* attachTo, const CFrame& attach, const PartProperties& properties, Entity entity) : Part(hitbox, *attachTo, attach, properties), entity(entity) {}



};