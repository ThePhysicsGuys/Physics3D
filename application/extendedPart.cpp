#include "core.h"

#include "application.h"
#include "extendedPart.h"
#include "ecs/components.h"
#include "view/screen.h"
#include "../engine/ecs/registry.h"
#include "../physics/misc/serialization.h"
#include "../engine/meshRegistry.h"

namespace P3D::Application {

ExtendedPart::ExtendedPart(Part&& part, VisualData visualData, const std::string& name, Entity entity) : 
	Part(std::move(part)), visualData(visualData), entity(entity) {
	if (this->entity == screen.registry.null_entity)
		this->entity = screen.registry.create();

	screen.registry.add<Comp::Model>(this->entity, this);
	if (!name.empty()) 
		screen.registry.add<Comp::Tag>(this->entity, name);
}

ExtendedPart::ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, VisualData visualData, const std::string& name, Entity entity) :
	Part(hitbox, position, properties), visualData(visualData), entity(entity) {
	if (this->entity == screen.registry.null_entity)
		this->entity = screen.registry.create();

	screen.registry.add<Comp::Model>(this->entity, this);
	if (!name.empty())
		screen.registry.add<Comp::Tag>(this->entity, name);
}

ExtendedPart::ExtendedPart(Part&& part, const std::string& name, Entity entity) :
	Part(std::move(part)), visualData(Engine::MeshRegistry::getOrCreateMeshFor(part.hitbox.baseShape)), entity(entity) {
	if (this->entity == screen.registry.null_entity)
		this->entity = screen.registry.create();

	screen.registry.add<Comp::Model>(this->entity, this);
	if (!name.empty())
		screen.registry.add<Comp::Tag>(this->entity, name);
}
ExtendedPart::ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, const std::string& name, Entity entity) :
	Part(hitbox, position, properties), visualData(Engine::MeshRegistry::getOrCreateMeshFor(hitbox.baseShape)), entity(entity) {
	if (this->entity == screen.registry.null_entity)
		this->entity = screen.registry.create();

	screen.registry.add<Comp::Model>(this->entity, this);
	if (!name.empty())
		screen.registry.add<Comp::Tag>(this->entity, name);
}

ExtendedPart::ExtendedPart(const Shape& hitbox, ExtendedPart* attachTo, const CFrame& attach, const PartProperties& properties, const std::string& name, Entity entity) :
	Part(hitbox, *attachTo, attach, properties), visualData(Engine::MeshRegistry::getOrCreateMeshFor(hitbox.baseShape)), entity(entity) {
	if (this->entity == screen.registry.null_entity)
		this->entity = screen.registry.create();

	screen.registry.add<Comp::Model>(this->entity, this);
	if (!name.empty())
		screen.registry.add<Comp::Tag>(this->entity, name);
}

void ExtendedPart::setMaterial(const Comp::Material& material) {
	screen.registry.add<Comp::Material>(this->entity, material);
}

};